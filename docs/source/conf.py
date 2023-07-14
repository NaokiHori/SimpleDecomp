import os
import sys

project = "SimpleDecomp"
author = "Naoki Hori"
copyright = f"2022, {author}"

sys.path.append(os.path.abspath("./ext"))
extensions = [
        "mydetails",
        "myliteralinclude",
]

html_theme = "alabaster"
html_theme_options = {
        "description": "Pencil-like domain decomposition",
        "fixed_sidebar": "false",
        "github_banner": "false",
        "github_button": "true",
        "github_count": "true",
        "github_repo": project,
        "github_type": "star",
        "github_user": "NaokiHori",
        "navigation_with_keys": "true",
        "nosidebar": "false",
        "page_width": "95vw",
        "show_powered_by": "true",
        "show_related": "false",
        "show_relbars": "false",
        "sidebar_collapse": "true",
        "sidebar_includehidden": "false",
        "sidebar_width": "20vw",
}

