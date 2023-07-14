from docutils import nodes
from docutils.parsers.rst import directives
from docutils.statemachine import StringList
from sphinx.util.docutils import SphinxDirective
from sphinx.directives.code import LiteralIncludeReader


def get_lines(filename, tag):
    # NOTE: assuming unique tag
    delimiter = "|"
    is_inside = False
    with open(filename, "r") as f:
        for cnt, line in enumerate(f):
            if not is_inside and tag in line:
                # exclude tag line, and the line number starts from 1
                l0 = cnt + 2
                is_inside = True
                if delimiter in line:
                    # extract number and get lower bound
                    line = line.strip()
                    nlines = int(line.split(delimiter)[1])
                    # compute l1
                    l1 = l0 + nlines - 1
                    return f"{l0}-{l1}"
            # estimate lower bound, assuming function declaration
            if is_inside and ");" in line:
                # function termination
                l1 = cnt + 1
                return f"{l0}-{l1}"
            if is_inside and "\n" == line:
                # find blank line (ad hoc, end of "block")
                l1 = cnt
                return f"{l0}-{l1}"
    # error handling
    msg = f"tag not found: {tag}"
    raise RuntimeError(msg)


def reshape_text(text):
    # count number of space of all lines
    #   and remove unnecessary space
    elements = text.split("\n")
    nspaces = -1
    for element in elements:
        for cnt, char in enumerate(element):
            if " " == char:
                continue
            nspaces = min(nspaces, cnt) if -1 != nspaces else cnt
    for cnt, element in enumerate(elements):
        elements[cnt] = element[nspaces:]
    text = "\n".join(elements)
    return text


def get_caption(filename):
    # caption, always show filename
    if "src/" in filename:
        # /../../src/<filename>.c -> src/<filename>.c
        delimiter = "src/"
    elif "include/" in filename:
        # /../../include/<filename>.h -> include/<filename>.h
        delimiter = "include/"
    caption = delimiter + filename.split(delimiter)[1]
    return caption


def container_wrapper(directive, literal_node, caption):
    container_node = nodes.container(
            "",
            literal_block=True,
            classes=["literal-block-wrapper"]
    )
    parsed = nodes.Element()
    directive.state.nested_parse(
            StringList([caption], source=""),
            directive.content_offset,
            parsed
    )
    if isinstance(parsed[0], nodes.system_message):
        msg = f"Invalid caption: {parsed[0].astext()}"
        raise ValueError(msg)
    elif isinstance(parsed[0], nodes.Element):
        caption_node = nodes.caption(
                parsed[0].rawsource,
                "",
                *parsed[0].children
        )
        caption_node.source = literal_node.source
        caption_node.line = literal_node.line
        container_node += caption_node
        container_node += literal_node
        return container_node
    else:
        raise RuntimeError


class MyLiteralInclude(SphinxDirective):
    """
    Customised LiteralInclude class
    to extract a function declaration or a part of code
    """
    has_content = False
    required_arguments = 1
    optional_arguments = 2
    final_argument_whitespace = True
    option_spec = {
        "language": directives.unchanged_required,
        # comment which identify where to extract
        "tag": directives.unchanged_required,
        # attach file name, extract from implicit argument
        "caption": directives.unchanged,
    }

    def run(self):
        document = self.state.document
        if not document.settings.file_insertion_enabled:
            return [document.reporter.warning(
                "File insertion disabled",
                line=self.lineno
            )]
        try:
            rel_filename, filename = self.env.relfn2path(self.arguments[0])
            self.env.note_dependency(rel_filename)
            self.options["lines"] = get_lines(filename, self.options["tag"])
            reader = LiteralIncludeReader(filename, self.options, self.config)
            text, lines = reader.read()
            text = reshape_text(text)
            retnode = nodes.literal_block(text, text, source=filename)
            retnode["force"] = "force" in self.options
            self.set_source_info(retnode)
            retnode["language"] = self.options["language"]
            retnode["linenos"] = False
            caption = get_caption(self.arguments[0])
            retnode = container_wrapper(self, retnode, caption)
            self.add_name(retnode)
            return [retnode]
        except Exception as exc:
            return [document.reporter.warning(exc, line=self.lineno)]


def setup(app):
    app.add_directive("myliteralinclude", MyLiteralInclude)
    return {
        "version": "0.1",
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
