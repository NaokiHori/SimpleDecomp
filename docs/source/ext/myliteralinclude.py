import os
from typing import List
from docutils import nodes
from docutils.nodes import Node
from docutils.parsers.rst import directives
from sphinx.directives import optional_int
from sphinx.util.docutils import SphinxDirective
from sphinx.util.typing import OptionSpec
from sphinx.directives.code import LiteralIncludeReader


def modify_lines(filename, tag):
    with open(filename, "r") as f:
        for lineno, line in enumerate(f):
            if "! {} !".format(tag) in line:
                num_extract = int(line.split("!")[2])
                break
    lineno += 2
    s = lineno
    e = lineno+num_extract-1
    return "{}-{}".format(s, e), s

class MyLiteralInclude(SphinxDirective):
    """
    Customised LiteralInclude class
    """
    has_content = False
    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec: OptionSpec = {
        'language': directives.unchanged_required,
        'tag': directives.unchanged_required,
    }
    def run(self) -> List[Node]:
        document = self.state.document
        if not document.settings.file_insertion_enabled:
            return [document.reporter.warning('File insertion disabled',
                                              line=self.lineno)]
        try:
            location = self.state_machine.get_source_and_line(self.lineno)
            rel_filename, filename = self.env.relfn2path(self.arguments[0])
            self.env.note_dependency(rel_filename)
            self.options['lines'], self.options['lineno-start'] = modify_lines(filename, self.options['tag'])
            reader = LiteralIncludeReader(filename, self.options, self.config)
            text, lines = reader.read(location=location)
            retnode: Element = nodes.literal_block(text, text, source=filename)
            retnode['force'] = 'force' in self.options
            self.set_source_info(retnode)
            retnode['language'] = self.options['language']
            retnode['linenos'] = True
            extra_args = retnode['highlight_args'] = {}
            extra_args['linenostart'] = reader.lineno_start
            self.add_name(retnode)
            return [retnode]
        except Exception as exc:
            return [document.reporter.warning(exc, line=self.lineno)]

def setup(app):
    app.add_directive("myliteralinclude", MyLiteralInclude)

    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
