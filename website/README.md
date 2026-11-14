# Eliot website

The English, French, and Italian Eliot website, built with Hugo using shared
HTML templates and CSS. The website builds independently of the application.

The content and appearance are based on the
[original Eliot website](https://www.nongnu.org/eliot/en/index.html).

## Requirements

Install the Hugo version recorded in `.hugo-version`.
The standard edition is sufficient; no theme, npm packages, or application
build dependencies are needed.

Run the following commands from this directory (`website/`).

## Local development

```sh
hugo server
```

Open <http://localhost:1313/>. Hugo rebuilds the site and reloads the browser
when files change. The root redirects to French; translations are available
at `/en/`, `/fr/`, and `/it/`.

## Production build

```sh
hugo --baseURL https://OWNER.github.io/REPOSITORY/ --minify --panicOnWarning --cleanDestinationDir
```

Replace the example URL with the actual public URL, including its trailing
slash. Use `https://OWNER.github.io/` for a site hosted at the domain root.
Templates and internal Markdown links support both forms.

The generated site is written to `public/`, which is ignored by Git.
For a local check of a project-path build, run:

```sh
hugo server --baseURL http://localhost:1313/eliot/ --appendPort=false
```

Then open <http://localhost:1313/eliot/>. Check navigation, language switching,
dictionary section links, images, and downloads before publishing.

## Structure and editing

- `content/en/`, `content/fr/`, `content/it/`: translated Markdown pages
- `i18n/`: translated navigation labels and footer text
- `layouts/`: shared document structure, partials, and shortcodes
- `static/layout/eliot.css`: site styling
- `static/img/`, `static/layout/`, `static/download/`: images and downloadable files
- `hugo.toml`: language configuration and site metadata

Keep corresponding translations under the same filename. Hugo uses those
filenames to connect translations, and the language selector opens the
translation of the current page. Each language's `_index.md` defines its
overview, including the descriptions used by the `game-modes` shortcode.

Write prose in Markdown and keep shared layout in templates. Use site-root
paths for internal links, such as `[Download](/en/download/)`; the link
renderer adds any deployment prefix. Preserve explicit heading IDs such as
`{#fr}` and existing front-matter aliases so bookmarks and older URLs work.

## Hosting

The generated `public/` directory can be published to GitHub Pages. Automated
deployment is not configured here yet. A repository-level GitHub Actions
workflow can build `website/` using the pinned Hugo version and publish the
output as a Pages artifact. Generated files do not need to be committed.
