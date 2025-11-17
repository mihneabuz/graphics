#!/usr/bin/python

from typing import TypedDict
import re


class Proc(TypedDict):
    name: str
    ftype: str
    ret: str
    args: list[tuple[str, str]]


class ProcGroup(TypedDict):
    version: str
    procs: list[Proc]


def print_header():
    with open("loader/header.h", "rt") as header_file:
        print(header_file.read())


def read_raw_procs() -> str:
    with open("loader/raw_procs", "rt") as raw_procs_file:
        return raw_procs_file.read()


def parse_proc_groups(raw_procs: str) -> list[ProcGroup]:
    split = raw_procs.split("// GL version ")[1:]

    groups = []
    for s in split:
        version, _, rest = s.partition('\n')
        groups.append({
            "version": version.replace(".", "_"),
            "procs": [parse_proc(raw_proc) for raw_proc in rest.split("\n\n") if raw_proc != "\n"],
        })

    return groups


def parse_proc(raw_proc: str) -> Proc:
    pattern = "typedef (.*) \\(\\*(\\w+)\\)\\((.*)\\);\\n(\\w+) (\\w+);"
    match = re.search(pattern, raw_proc)

    if not match:
        print(f"failed to parse proc:\n{raw_proc}")
        exit(1)

    args = []
    args_str = match.group(3)
    if args_str != "void":
        for arg in args_str.split(", "):
            typ, _, name = arg.rpartition(' ')
            args.append((typ, name))

    proc: Proc = {
        "name": match.group(5),
        "ftype": match.group(2),
        "ret": match.group(1),
        "args": args
    }

    return proc

def generate_signatures(groups: list[ProcGroup]):
    for group in groups:
        for proc in group["procs"]:
            arg_list = ", ".join([f"{arg[0]} {arg[1]}" for arg in proc["args"]])
            print(f"{proc["ret"]} {proc["name"]}({arg_list});")


def generate_group_loader(group: ProcGroup):
    print(f"// OpenGL version {group["version"]}\n")
    for proc in group["procs"]:
        generate_proc(proc)

    print(f"static inline void load_gl_{group["version"]}(ProcLoader load) {{")
    for proc in group["procs"]:
        print(f"    proc_{proc["name"]} = load(\"{proc["name"]}\");")
    print("}")
    print()


def generate_proc(proc: Proc):
    arg_list = ", ".join([f"{arg[0]} {arg[1]}" for arg in proc["args"]])
    arg_syms = ", ".join([arg[1] for arg in proc["args"]])
    print(f"typedef {proc["ret"]} (*{proc["ftype"]})({arg_list});");
    print(f"{proc["ftype"]} proc_{proc["name"]};");
    print(f"inline {proc["ret"]} {proc["name"]}({arg_list}) {{")

    def formatter(typ: str):
        formatters = {
            "GLchar": "%d",
            "GLbyte": "%d",
            "GLubyte": "%d",
            "GLshort": "%d",
            "GLushort": "%u",
            "GLint": "%d",
            "GLuint": "%u",
            "GLsizei": "%d",
            "GLint64": "%ld",
            "GLuint64": "%lu",
            "GLintptr": "%ld",
            "GLsizeiptr": "%ld",
            "GLfloat": "%f",
            "GLdouble": "%lf",
            "GLenum": "%d",
            "GLboolean": "%d",
            "GLbitfield": "%u",
            "GLsync": "%p",

            "GLchar*": "'%s'",
            "const GLchar*": "'%s'",
            "GLbyte*": "'%s'",
            "const GLbyte*": "'%s'",
            "GLubyte*": "'%s'",
            "const GLubyte*": "'%s'",
        }

        if typ in formatters:
            return formatters[typ]

        if typ.endswith('*'):
            return "%p"

        print(f"failed to format type {typ}")
        exit(1)

    arg_format_str = ", ".join([formatter(arg[0]) for arg in proc["args"]])
    arg_format_vars = f", {arg_syms}" if arg_format_str else ""

    if (proc["ret"] == "void"):
        print("#ifdef GL_DEBUG")
        print(f"    fprintf(stderr, \"{proc["name"]}({arg_format_str})\\n\"{arg_format_vars});")
        print("#endif")
        print(f"    proc_{proc["name"]}({arg_syms});")
    else:
        print("#ifdef GL_DEBUG")
        print(f"    fprintf(stderr, \"{proc["name"]}({arg_format_str})\"{arg_format_vars});")
        print("#endif")
        print(f"    {proc["ret"]} ret = proc_{proc["name"]}({arg_syms});")
        print("#ifdef GL_DEBUG")
        print(f"    fprintf(stderr, \" = {formatter(proc["ret"])}\\n\", ret);")
        print("#endif")
        print(f"    return ret;")
    print("}")
    print()


def generate_meta_loader(groups: list[ProcGroup]):
    for group in groups:
        generate_group_loader(group)

    print("static inline int load_gl_procs(ProcLoader load) {")
    for group in groups:
        print(f"    load_gl_{group["version"]}(load);")
    print("    if (proc_glGetString == NULL) return 0;")
    print("    if (glGetString(GL_VERSION) == NULL) return 0;");
    print("    return 1;")
    print("}")


if __name__ == "__main__":
    raw_procs = read_raw_procs()
    groups = parse_proc_groups(raw_procs)

    print("#ifndef GL_INCLUDE")
    print("#define GL_INCLUDE")
    print_header()
    generate_signatures(groups)
    print("#endif")

    print("#ifdef GL_LOADER")
    generate_meta_loader(groups)
    print("#undef GL_LOADER")
    print("#endif")
