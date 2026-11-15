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
cp public/fr/404.html public/404.html
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

## Missing pages

The shared `layouts/404.html` template generates an error page in each language,
with links to home, installation, dictionaries, and help. The language selector
switches between these error pages.

GitHub Pages serves one root `404.html` for all missing URLs. The production
commands above and CI copy the French error page there, matching the site's
default language. Its stylesheet and recovery links work from nested missing
URLs, including under `/eliot/`. `hugo server` uses the same French fallback;
the language selector offers English and Italian. Test a nonexistent
URL such as `/en/missing/page/`; it should return HTTP 404 with the custom page.

## Validate changes

After changing templates or styles, check representative pages in each language
with the [W3C HTML Checker](https://validator.w3.org/nu/) and the
[W3C CSS Validator](https://jigsaw.w3.org/css-validator/). Submit a deployed page
URL or upload a generated file from `public/`. Include a long dictionary/archive
page when checking layout and structure.

## Structure and editing

- `content/en/`, `content/fr/`, `content/it/`: translated Markdown pages
- `i18n/`: translated navigation labels and footer text
- `data/screenshots.yaml`: shared screenshot inventory, grouped by version
- `layouts/`: shared document structure, partials, and shortcodes
- `static/layout/eliot.css`: site styling
- `static/img/`, `static/layout/`, `static/download/`: images and downloadable files
- `hugo.toml`: language configuration and site metadata

Keep corresponding translations under the same filename. Hugo uses those
filenames to connect translations, and the language selector opens the
translation of the current page. Each language's `install.md` defines the
installation overview at `/<language>/install/`; `/download/` and
`download.html` aliases within each language preserve older links.
Installation pages use `type: installation` for shared list and command styling.
The overview covers software, dictionaries, and first launch; platform/source
pages use `{{< installation-next >}}` for shared onward and email-help links.
Ordinary installation, dictionary, startup, and usage help belongs on
`eliot-general`; compiling from source and development belong on `eliot-dev`.
The Build from source page uses `{{< installation-next build=true >}}` to
select build help. Its `dl-sources.md` combines source downloads and guidance
for other operating systems; `/dl-others/` and `/dl-others.html` remain aliases
within each language, alongside `/dl-sources.html`.
Keep menu names in the first-launch instructions consistent with the application's
translation catalogs. Historical package compatibility and current development
build requirements describe different versions; keep that distinction explicit.
Each language's `_index.md` defines its overview, including the descriptions
used by the `game-modes` shortcode.

Each language's `news.md` contains the full historical news archive.

The `dictionary-index` shortcode lists the sections of the current language's
`dict.md`, using its heading labels and IDs. Use it on dictionary and
installation pages; adding a dictionary section automatically updates both indexes.

The `screenshots` shortcode renders the shared gallery from `data/screenshots.yaml`
and the captions in each language's `screenshots.md`. Add original images to
`static/img/`; Hugo also mounts this directory as image resources and generates
WebP thumbnails during the build. Full-size image URLs stay unchanged; generated
thumbnails do not need to be committed.
The homepages' `screenshotsAnchor` values preserve former screenshot-section
bookmarks on the gallery menu link.

Hugo fingerprints the stylesheet from `static/layout/eliot.css`, so CSS changes
get a new URL and browsers fetch the matching styles after an update.

The current page and language are highlighted in navigation. If a translation
is missing, its language name is shown without a link, with an explanation on
hover; it never silently falls back to a different page.

The main menu focuses on Home, Install, Dictionaries, Screenshots, Contribute,
and Help. Help pages (`help.md`) cover installation, bug reports, and email
lists. Resources (`resources.md`) and the news archive are linked
from the footer. Former `contact/`, `contact.html`, `links/`, and `links.html`
URLs remain aliases within each language; preserve their existing section IDs.

Write prose in Markdown and keep shared layout in templates. Use site-root
paths for internal links, such as `[Install](/en/install/)`; the link
renderer adds any deployment prefix. Preserve explicit heading IDs such as
`{#fr}` and existing front-matter aliases so bookmarks and older URLs work.

## Editorial conventions

Use these game-mode names in prose; capitalize them at the start of a sentence
or when quoting an interface label:

| English | French | Italian |
| --- | --- | --- |
| classic | classique | classico |
| duplicate | duplicate | duplicato |
| training | entraînement | allenamento |
| arbitration | arbitrage | arbitrato |
| topping | topping | topping |

Introduce the current application label once on each localized homepage, then
use the terms above throughout the website. In Italian prose, use *modalità
classica* and *partita classica*.

Distinguish the tile from the rule variant, which is separate from the main
game modes. Use these terms in prose; the English and Italian variant names
are descriptive labels:

| Meaning | English | French | Italian |
| --- | --- | --- | --- |
| Tile | blank / blank tile | joker | jolly |
| Rule variant | blank-tile variant | partie joker | variante jolly |

- Write dates as day, spelled-out month, and year: `15 September 2020`,
  `15 septembre 2020`, `15 settembre 2020`. Omit leading zeros; use lowercase
  month names in French and Italian. Preserve existing heading IDs when editing
  dated announcements so bookmarks keep working.
- Use `Mac OS X` for the historical packages and `Mac OS` for classic versions
  such as 8.5. Keep product names such as Eliot, Git, Windows, and Linux Mint
  consistently capitalized. For published releases, prefer "release" in English,
  "version" in French, and "versione" in Italian.
- A word list (liste de mots/lista di parole) is the source vocabulary; an Eliot
  dictionary (dictionnaire/dizionario) is a compiled `.dawg` file. Dictionary
  languages, application interface translations, and website translations are
  independent choices.
- Italicize interface labels and match the application's wording in `qt/ui/`
  and `po/`. Use inline code for commands, filenames, and program names.

## Project links

The public website is <https://ipkiss42.github.io/eliot/>. Set the repository's
**About → Website** field on GitHub to this URL as well; that repository setting
is managed separately from Git commits.

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
