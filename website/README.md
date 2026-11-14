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

The repository's `.github/workflows/website.yml` builds the site using the
version in `.hugo-version`. Pull requests targeting `main` build without
deploying. Changes merged or pushed to `main` build and deploy automatically
when they affect `website/` or the website workflow. Website-only changes do
not trigger the application's Linux/Windows builds.

To enable publishing, select **GitHub Actions** in the repository's
**Settings → Pages → Build and deployment → Source**. If the `github-pages`
environment has deployment branch restrictions, allow `main`. See
[GitHub's Pages workflow documentation](https://docs.github.com/en/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages).

Deployment builds use the public URL supplied by GitHub Pages, including any
project path or custom domain. Pull requests use an example project-path URL
and can build before Pages is enabled. The workflow can also be run manually
from the Actions tab; only runs on `main` deploy. Generated files are uploaded
as a Pages artifact and do not need to be committed.
