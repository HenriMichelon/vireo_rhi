#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_lua_docs.py
===============

Generates HTML documentation for Lua bindings from EmmyLua annotations.

Usage
-----
    python gen_lua_docs.py [--lua annotations.lua] [--out output_dir]
                           [--project-brief "My project description"]
                           [--head-template header.html] [--foot-template footer.html]
"""

from __future__ import annotations

import argparse
import datetime
import html
import os
import re
import sys
from dataclasses import dataclass, field
from typing import Optional

@dataclass
class LuaField:
    name: str
    raw_type: str            # raw EmmyLua type, e.g. "fun(self:vireo.Fence):nil"
    doc: str = ""            # descriptive comment (preceding --- lines)
    is_method: bool = False  # True if the type is a function
    static: bool = False     # @static found in inline note
    inline_note: str = ""    # note appended at end of line (e.g. @read-only)
    params: list[tuple[str, str]] = field(default_factory=list)  # (name, type)
    returns: str = ""
@dataclass
class LuaClass:
    name: str                # full name, e.g. "vireo.Fence"
    short: str               # short name, e.g. "Fence"
    parent: Optional[str] = None   # inheritance ": vireo.Pipeline"
    doc: str = ""            # description (--- lines before @class)
    fields: list[LuaField] = field(default_factory=list)
    kind: str = "class"      # "class", "enum" or "struct"

    def page_name(self) -> str:
        return self.short + "_lua.html"
@dataclass
class DocContext:
    """Project-level metadata threaded through all rendering functions."""
    project_name: str    # capitalised namespace name, e.g. "Vireo"
    project_brief: str   # one-line project description for the page header
    ns_name: str         # raw namespace name, e.g. "vireo"
    ns_href: str         # namespace page filename, e.g. "namespacevireo_lua.html"
    head_tmpl: str = ""  # content of the HTML head/header template file
    foot_tmpl: str = ""  # content of the HTML footer template file

RE_CLASS     = re.compile(r"^---@class\s+([A-Za-z0-9_.]+)\s*(?::\s*([A-Za-z0-9_.]+))?\s*(?:\s+(.+))?$")
RE_FIELD     = re.compile(r"^---@field\s+(\S+)\s+(.*?)\s*$")
RE_TYPE      = re.compile(r"^---@type\s+([A-Za-z0-9_.]+)\s*$")
RE_ALIAS     = re.compile(r"^---@alias\s+([A-Za-z0-9_.]+)\s+(.*?)\s*$")
RE_DOC       = re.compile(r"^---\s?(.*)$")
RE_DIRECTIVE = re.compile(r"^---@")
def _split_inline_note(rest: str) -> tuple[str, str]:
    m = re.search(r"\s+@(.*)$", rest)
    if m:
        return rest[: m.start()].rstrip(), m.group(1).strip()
    return rest, ""
def _split_type_and_doc(rest: str) -> tuple[str, str]:
    rest = rest.strip()
    if rest.startswith("fun"):
        depth = 0
        i = 0
        while i < len(rest):
            if rest[i] == "(":
                depth += 1
            elif rest[i] == ")":
                depth -= 1
                if depth == 0:
                    i += 1
                    break
            i += 1
        if i < len(rest) and rest[i] == ":":
            i += 1
            while i < len(rest) and rest[i] == " ":
                i += 1
            # consume return type word (may contain dots, brackets, pipes, |nil combos)
            while i < len(rest) and rest[i] not in (" ", "\t"):
                i += 1
        raw_type = rest[:i].rstrip()
        remainder = rest[i:].lstrip()
    else:
        parts = rest.split(None, 1)
        raw_type = parts[0]
        remainder = parts[1] if len(parts) > 1 else ""

    m = re.search(r"\s+@\S", remainder)
    if m:
        inline_doc = remainder[:m.start()].strip()
        inline_note = remainder[m.start():].strip().lstrip("@").strip()
    else:
        inline_doc = remainder.strip()
        inline_note = ""

    if inline_doc.startswith("@"):
        inline_note = inline_doc.lstrip("@").strip()
        inline_doc = ""

    return raw_type, inline_doc, inline_note
def _parse_fun(t: str) -> tuple[list[tuple[str, str]], str]:
    m = re.match(r"^fun\s*\((.*)\)\s*(?::\s*(.*))?$", t.strip())
    if not m:
        return [], ""
    args_str = m.group(1).strip()
    ret = (m.group(2) or "").strip()
    params: list[tuple[str, str]] = []
    if args_str:
        for arg in _split_top_level(args_str, ","):
            arg = arg.strip()
            if not arg:
                continue
            if ":" in arg:
                pname, ptype = arg.split(":", 1)
                params.append((pname.strip(), ptype.strip()))
            else:
                params.append((arg, ""))
    return params, ret
def _split_top_level(s: str, sep: str) -> list[str]:
    out: list[str] = []
    depth = 0
    cur = []
    for ch in s:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == sep and depth == 0:
            out.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    out.append("".join(cur))
    return out
def parse_lua(path: str):
    with open(path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    classes: dict[str, LuaClass] = {}
    aliases: dict[str, str] = {}
    type_globals: list[str] = []

    current: Optional[LuaClass] = None
    pending_doc: list[str] = []

    def flush_doc() -> str:
        nonlocal pending_doc
        text = "\n".join(pending_doc).strip()
        pending_doc = []
        return text

    for raw in lines:
        line = raw.rstrip("\n")
        stripped = line.strip()

        m = RE_CLASS.match(stripped)
        if m:
            full = m.group(1)
            parent = m.group(2)
            inline_class_doc = (m.group(3) or "").strip()
            short = full.split(".")[-1]
            preceding_doc = flush_doc()
            cls_doc = preceding_doc or inline_class_doc
            cls = LuaClass(name=full, short=short, parent=parent, doc=cls_doc)
            classes[full] = cls
            current = cls
            continue

        m = RE_FIELD.match(stripped)
        if m and current is not None:
            fname = m.group(1)
            rest = m.group(2)
            ftype, field_inline_doc, inline = _split_type_and_doc(rest)
            preceding_doc = flush_doc()
            fdoc = preceding_doc or field_inline_doc
            fld = LuaField(name=fname, raw_type=ftype, doc=fdoc, inline_note=inline)
            if ftype.startswith("fun"):
                fld.is_method = True
                fld.params, fld.returns = _parse_fun(ftype)
            note = inline.lower()
            if "static" in note or "static" in ftype.lower():
                fld.static = True
            current.fields.append(fld)
            continue

        m = RE_TYPE.match(stripped)
        if m:
            type_globals.append(m.group(1))
            flush_doc()
            current = None
            continue

        m = RE_ALIAS.match(stripped)
        if m:
            aliases[m.group(1)] = m.group(2)
            flush_doc()
            continue

        if RE_DIRECTIVE.match(stripped):
            flush_doc()
            continue

        m = RE_DOC.match(stripped)
        if m:
            content = m.group(1)
            if content and set(content) <= set("-"):
                pending_doc = []
            else:
                pending_doc.append(content)
            continue

        pending_doc = []

    return classes, aliases, type_globals

def classify(cls: LuaClass) -> str:
    if not cls.fields:
        return "class"
    has_method = any(f.is_method for f in cls.fields)
    if has_method:
        return "class"

    def is_const_name(n: str) -> bool:
        return bool(re.fullmatch(r"[A-Z0-9_]+", n))

    all_integer = all(
        (not f.is_method) and f.raw_type.strip() == "integer"
        for f in cls.fields
    )
    all_upper = all(is_const_name(f.name) for f in cls.fields)
    if all_integer and all_upper:
        return "enum"
    return "struct"

def esc(s: str) -> str:
    return html.escape(s, quote=False)
def _footer_date() -> str:
    return datetime.datetime.now().strftime("%a %b %d %Y %H:%M:%S")

def render_head(title: str, is_namespace: bool, ctx: DocContext) -> str:
    return ctx.head_tmpl.format(
        project_name=esc(ctx.project_name),
        project_brief=esc(ctx.project_brief),
        ns_href=ctx.ns_href,
        title=esc(title),
        ns_current=' class="current"' if is_namespace else "",
    )
def render_footer(navpath_items: list[tuple[str, str]], ctx: DocContext) -> str:
    parts = [
        '    <li class="navelem"><a class="el" href="%s">%s</a></li>' % (href, esc(label))
        for href, label in navpath_items
    ]
    return ctx.foot_tmpl.format(
        navpath="\n".join(parts),
        date=_footer_date(),
        project_name=esc(ctx.project_name),
    )
def type_link(t: str, classes: dict[str, LuaClass]) -> str:
    t = t.strip()
    if not t:
        return ""
    if "|" in t:
        return "|".join(type_link(p, classes) for p in t.split("|"))
    suffix = ""
    while t.endswith("[]"):
        suffix += "[]"
        t = t[:-2].strip()
    base = t
    if base in classes:
        html_t = '<a class="el" href="%s">%s</a>' % (
            classes[base].page_name(), esc(classes[base].short))
    else:
        html_t = esc(base)
    return html_t + esc(suffix)
def method_signature(fld: LuaField, classes: dict[str, LuaClass]) -> str:
    params = [p for p in fld.params if p[0] != "self"]
    rendered = []
    for pname, ptype in params:
        if ptype:
            rendered.append('%s: %s' % (esc(pname), type_link(ptype, classes)))
        else:
            rendered.append(esc(pname))
    return "(" + ", ".join(rendered) + ")"

def render_namespace(global_cls: LuaClass, classes: dict[str, LuaClass],
                     ctx: DocContext) -> str:
    title = "%s Lua Namespace Reference" % ctx.ns_name
    out = [render_head(title, is_namespace=True, ctx=ctx)]

    out.append('<div class="header">')
    out.append('  <div class="summary">')
    out.append('<a href="#enum-members">Enums &amp; Constants</a> &#124;')
    out.append('<a href="#struct-members">Structures</a> &#124;')
    out.append('<a href="#class-members">Classes</a>  </div>')
    out.append('  <div class="headertitle">')
    out.append('    <div class="title">%s</div>' % esc(title))
    out.append('  </div>')
    out.append('  <div class="clear-floats"></div>')
    out.append('</div><!--header-->')
    out.append('<div class="contents">')

    if global_cls.doc:
        out.append('<div class="textblock"><p>%s</p></div>'
                   % esc(global_cls.doc).replace("\n", "<br />\n"))
    else:
        out.append(
            '<div class="textblock"><p>Global Lua namespace <code>%s</code>. '
            'Each entry below is a class accessible from the global table '
            '<code>%s</code>.</p></div>' % (esc(ctx.ns_name), esc(ctx.ns_name))
        )

    groups: dict[str, list] = {"enum": [], "struct": [], "class": []}
    for fld in global_cls.fields:
        target = classes.get(fld.raw_type.strip())
        groups["class" if target is None else target.kind].append((fld, target))

    section_titles = [
        ("enum",   "enum-members",   "Enums &amp; Constants"),
        ("struct", "struct-members", "Structures"),
        ("class",  "class-members",  "Classes"),
    ]

    out.append('<table class="memberdecls">')
    for kind, anchor, section_title in section_titles:
        items = groups[kind]
        if not items:
            continue
        out.append('<tr class="heading"><td colspan="2">'
                   '<h2 class="groupheader"><a name="%s"></a>' % anchor)
        out.append('%s</h2></td></tr>' % section_title)
        for fld, target in sorted(items, key=lambda it: it[0].name.lower()):
            if target is not None:
                link = '<a class="el" href="%s">%s</a>' % (
                    target.page_name(), esc(fld.name))
                desc = target.doc
            else:
                link = esc(fld.name)
                desc = ""
            out.append('<tr class="memitem:"><td class="memItemLeft" '
                       'align="right" valign="top">%s &#160;</td>'
                       '<td class="memItemRight" valign="bottom">%s</td></tr>'
                       % (kind, link))
            if desc:
                out.append('<tr class="memdesc:"><td class="mdescLeft">&#160;</td>'
                           '<td class="mdescRight">%s</td></tr>'
                           % esc(desc.split("\n")[0]))
            out.append('<tr class="separator:"><td class="memSeparator" '
                       'colspan="2">&#160;</td></tr>')
    out.append('</table>')

    out.append('</div><!-- contents -->')
    out.append(render_footer([(ctx.ns_href, ctx.ns_name)], ctx))
    return "\n".join(out)

def render_class(cls: LuaClass, classes: dict[str, LuaClass],
                 ctx: DocContext) -> str:
    label = {"enum": "Enum", "struct": "Struct", "class": "Class"}[cls.kind]
    out = [render_head("%s %s Reference" % (cls.short, label),
                       is_namespace=False, ctx=ctx)]

    methods = [f for f in cls.fields if f.is_method]
    props   = [f for f in cls.fields if not f.is_method]

    out.append('<div class="header">')
    out.append('  <div class="summary">')
    summ = []
    if methods:
        summ.append('<a href="#pub-methods">Public Member Functions</a>')
    if props:
        kind_anchor = "enum-values" if cls.kind == "enum" else "properties"
        kind_label  = "Constants"   if cls.kind == "enum" else "Properties"
        summ.append('<a href="#%s">%s</a>' % (kind_anchor, kind_label))
    out.append(" &#124;\n".join(summ) + "  </div>")
    out.append('  <div class="headertitle">')
    mlabel = ""
    if cls.kind == "enum":
        mlabel = '<span class="mlabels"><span class="mlabel">enum</span></span>'
    elif cls.kind == "class":
        mlabel = '<span class="mlabels"><span class="mlabel">abstract</span></span>'
    out.append('    <div class="title">%s %s Reference%s</div>'
               % (esc(cls.short), label, mlabel))
    out.append('  </div>')
    out.append('  <div class="clear-floats"></div>')
    out.append('</div><!--header-->')

    out.append('<div class="contents">')

    out.append('<a name="details" id="details"></a>'
               '<h2 class="groupheader">Detailed Description</h2>')
    if cls.doc:
        desc_html = esc(cls.doc).replace("\n", "<br />\n")
    else:
        desc_html = "Lua type <code>%s</code>." % esc(cls.name)
    out.append('<div class="textblock"><p>%s</p>' % desc_html)
    if cls.parent and cls.parent in classes:
        p = classes[cls.parent]
        out.append('<p>Inherits from: <a class="el" href="%s">%s</a></p>'
                   % (p.page_name(), esc(p.short)))
    out.append('<p>Lua full name: <code>%s</code></p>' % esc(cls.name))
    out.append('</div>')

    out.append('<table class="memberdecls">')

    if methods:
        out.append('<tr class="heading"><td colspan="2">'
                   '<h2 class="groupheader"><a name="pub-methods"></a>')
        out.append('Public Member Functions</h2></td></tr>')
        for m in methods:
            ret    = type_link(m.returns, classes) if m.returns else "nil"
            anchor = "lua_%s" % m.name
            sig    = method_signature(m, classes)
            right  = '<a class="el" href="#%s">%s</a> %s' % (anchor, esc(m.name), sig)
            if m.static:
                right += '<span class="mlabels"><span class="mlabel">static</span></span>'
            out.append('<tr class="memitem:%s"><td class="memItemLeft" '
                       'align="right" valign="top">%s&#160;</td>'
                       '<td class="memItemRight" valign="bottom">%s</td></tr>'
                       % (anchor, ret, right))
            if m.doc:
                out.append('<tr class="memdesc:%s"><td class="mdescLeft">&#160;</td>'
                           '<td class="mdescRight">%s <a href="#%s">More...</a><br /></td></tr>'
                           % (anchor, esc(m.doc.split(chr(10))[0]), anchor))
            out.append('<tr class="separator:%s"><td class="memSeparator" '
                       'colspan="2">&#160;</td></tr>' % anchor)

    if props:
        kind_anchor = "enum-values" if cls.kind == "enum" else "properties"
        kind_label  = "Constants"   if cls.kind == "enum" else "Properties"
        out.append('<tr class="heading"><td colspan="2">'
                   '<h2 class="groupheader"><a name="%s"></a>' % kind_anchor)
        out.append('%s</h2></td></tr>' % kind_label)
        for p in props:
            anchor = "lua_%s" % p.name
            right  = '<a class="el" href="#%s">%s</a>' % (anchor, esc(p.name))
            if p.inline_note:
                right += ('&#160;<span class="mlabels"><span class="mlabel">%s</span></span>'
                          % esc(p.inline_note))
            out.append('<tr class="memitem:%s"><td class="memItemLeft" '
                       'align="right" valign="top">%s&#160;</td>'
                       '<td class="memItemRight" valign="bottom">%s</td></tr>'
                       % (anchor, type_link(p.raw_type, classes), right))
            if p.doc:
                out.append('<tr class="memdesc:%s"><td class="mdescLeft">&#160;</td>'
                           '<td class="mdescRight">%s<br /></td></tr>'
                           % (anchor, esc(p.doc.split(chr(10))[0])))
            out.append('<tr class="separator:%s"><td class="memSeparator" '
                       'colspan="2">&#160;</td></tr>' % anchor)

    out.append('</table>')

    if methods:
        out.append('<h2 class="groupheader">Member Function Documentation</h2>')
        for m in methods:
            anchor = "lua_%s" % m.name
            ret    = type_link(m.returns, classes) if m.returns else "nil"
            out.append('<a class="anchor" id="%s"></a>' % anchor)
            out.append('<div class="memitem">')
            out.append('<div class="memproto">')
            out.append('<table class="mlabels">')
            out.append('  <tr>')
            out.append('  <td class="mlabels-left">')
            out.append('      <table class="memname">')
            params = [p for p in m.params if p[0] != "self"]
            if not params:
                out.append('        <tr>')
                out.append('          <td class="memname">%s %s </td>' % (ret, esc(m.name)))
                out.append('          <td>(</td>')
                out.append('          <td class="paramname"></td><td>)</td>')
                out.append('          <td></td>')
                out.append('        </tr>')
            else:
                for i, (pn, pt) in enumerate(params):
                    namecell = ('<td class="memname">%s %s </td><td>(</td>'
                                % (ret, esc(m.name))) if i == 0 else \
                               '<td class="memname"></td><td></td>'
                    closing  = ')' if i == len(params) - 1 else ','
                    out.append('        <tr>')
                    out.append('          %s' % namecell)
                    out.append('          <td class="paramtype">%s&#160;</td>'
                               % (type_link(pt, classes) if pt else ""))
                    out.append('          <td class="paramname">%s</td><td>%s</td>'
                               % (esc(pn), closing))
                    out.append('          <td></td>')
                    out.append('        </tr>')
            out.append('      </table>')
            out.append('  </td>')
            if m.static:
                out.append('  <td class="mlabels-right">'
                           '<span class="mlabels"><span class="mlabel">static</span>'
                           '</span>  </td>')
            out.append('  </tr>')
            out.append('</table>')
            out.append('</div><div class="memdoc">')
            if m.doc:
                out.append('<p>%s</p>' % esc(m.doc).replace("\n", "<br />\n"))
            if params:
                out.append('<dl class="params"><dt>Parameters</dt><dd>')
                out.append('  <table class="params">')
                for pn, pt in params:
                    out.append('    <tr><td class="paramname">%s</td><td>%s</td></tr>'
                               % (esc(pn), type_link(pt, classes)))
                out.append('  </table></dd></dl>')
            if m.returns and m.returns != "nil":
                out.append('<dl class="section return"><dt>Returns</dt>'
                           '<dd>%s</dd></dl>' % type_link(m.returns, classes))
            out.append('</div></div>')

    if props:
        doc_title = ("Enumerator Documentation" if cls.kind == "enum"
                     else "Property Documentation")
        out.append('<h2 class="groupheader">%s</h2>' % doc_title)
        for p in props:
            anchor = "lua_%s" % p.name
            out.append('<a class="anchor" id="%s"></a>' % anchor)
            out.append('<div class="memitem"><div class="memproto">')
            out.append('  <table class="memname"><tr>')
            out.append('    <td class="memname">%s %s</td>'
                       % (type_link(p.raw_type, classes), esc(p.name)))
            out.append('  </tr></table>')
            out.append('</div><div class="memdoc">')
            if p.doc:
                out.append('<p>%s</p>' % esc(p.doc).replace("\n", "<br />\n"))
            out.append('</div></div>')

    out.append('</div><!-- contents -->')
    out.append(render_footer(
        [(ctx.ns_href, ctx.ns_name), (cls.page_name(), cls.short)], ctx))
    return "\n".join(out)

def main(argv=None) -> int:
    ap = argparse.ArgumentParser(
        description="Lua binding HTML documentation generator.")
    ap.add_argument("--lua", default="src/bindings/lua/vireo.lua",
                    help="EmmyLua annotation file (default: src/bindings/lua/vireo.lua)")
    ap.add_argument("--out", default="docs/html",
                    help="Output directory (default: docs/html)")
    ap.add_argument("--project-brief", default="", dest="project_brief",
                    metavar="TEXT",
                    help="One-line project description shown in the page header")
    ap.add_argument("--head-template", default="", dest="head_template",
                    metavar="FILE",
                    help="Path to the HTML head/header template file")
    ap.add_argument("--foot-template", default="", dest="foot_template",
                    metavar="FILE",
                    help="Path to the HTML footer template file")
    args = ap.parse_args(argv)

    if not os.path.isfile(args.lua):
        print("Error: file not found: %s" % args.lua, file=sys.stderr)
        return 1

    def _read_template(path: str, label: str) -> str:
        if not path:
            return ""
        if not os.path.isfile(path):
            print("Error: %s not found: %s" % (label, path), file=sys.stderr)
            raise SystemExit(1)
        with open(path, "r", encoding="utf-8") as fh:
            return fh.read()

    head_tmpl = _read_template(args.head_template, "head template")
    foot_tmpl = _read_template(args.foot_template, "foot template")

    os.makedirs(args.out, exist_ok=True)

    classes, aliases, type_globals = parse_lua(args.lua)

    if not type_globals:
        print("Error: no ---@type annotation found; cannot determine namespace name.",
              file=sys.stderr)
        return 1

    unknown = [t for t in type_globals if t not in classes]
    if unknown:
        print("Error: ---@type names not declared as @class: %s" % ", ".join(unknown),
              file=sys.stderr)
        return 1

    ns_set = set(type_globals)
    for name, cls in classes.items():
        if name not in ns_set:
            cls.kind = classify(cls)

    project_name  = type_globals[0].capitalize()
    project_brief = args.project_brief or ("%s Lua API" % project_name)

    for ns_name in type_globals:
        global_cls = classes[ns_name]
        ns_href    = "namespace%s_lua.html" % ns_name
        ctx = DocContext(
            project_name  = project_name,
            project_brief = project_brief,
            ns_name       = ns_name,
            ns_href       = ns_href,
            head_tmpl     = head_tmpl,
            foot_tmpl     = foot_tmpl,
        )
        ns_path = os.path.join(args.out, ns_href)
        with open(ns_path, "w", encoding="utf-8") as f:
            f.write(render_namespace(global_cls, classes, ctx))
        print("Write: %s" % ns_path)

    first_ns   = type_globals[0]
    first_href = "namespace%s_lua.html" % first_ns
    ctx = DocContext(
        project_name  = project_name,
        project_brief = project_brief,
        ns_name       = first_ns,
        ns_href       = first_href,
        head_tmpl     = head_tmpl,
        foot_tmpl     = foot_tmpl,
    )
    count = 0
    for name, cls in classes.items():
        if name in ns_set:
            continue
        p = os.path.join(args.out, cls.page_name())
        with open(p, "w", encoding="utf-8") as f:
            f.write(render_class(cls, classes, ctx))
        count += 1
    print("Write: %d class pages in %s" % (count, args.out))

    return 0
if __name__ == "__main__":
    raise SystemExit(main())
