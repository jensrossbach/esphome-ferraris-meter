from datetime import datetime

project = 'ESPHome Ferraris Meter'
copyright = '2024-2026, Jens-Uwe Rossbach'
author = 'Jens-Uwe Rossbach'

templates_path = ['_templates']
exclude_patterns = []

highlight_language = 'yaml'

html_static_path = ['_static']
html_theme = 'pydata_sphinx_theme'
html_theme_options = {
    'logo': {
        'image_light': 'ferraris_meter_icon.png',
        'image_dark':  'ferraris_meter_icon.png',
        'text':        'Ferraris Meter'
    },
    "icon_links": [
        {
            "name": "GitHub",
            "url": "https://github.com/jensrossbach/esphome-ferraris-meter",
            "icon": "fa-brands fa-github",
        },
    ],
    'secondary_sidebar_items': {
        '**/*': ['page-toc']
    },
    "footer_start": ["copyright"],
    "footer_end": ["language"]
}

html_context = {
    'default_mode': 'auto',
    "project": project,
    "author": author,
    "copyright": copyright,
    "year": datetime.now().year,
    "languages": {
        "de": "/de/index.html",
        "en": "/en/index.html"
    }
}

html_css_files = [
    'css/theme.css'
]

rst_epilog = """
.. |nbsp| unicode:: U+00A0
   :trim:
   :ltrim:
"""

locale_dirs = ['locale/']
gettext_compact = False
