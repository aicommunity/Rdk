# Markdown JavaScript Libraries

## RU

Эта папка должна содержать следующие JavaScript библиотеки для рендеринга Markdown:

1. **marked.min.js** - библиотека для конвертации Markdown в HTML
   - Скачать с: https://cdn.jsdelivr.net/npm/marked/marked.min.js
   - Или: https://github.com/markedjs/marked/releases

2. **mermaid.min.js** - библиотека для рендеринга Mermaid диаграмм
   - Скачать с: https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js
   - Или: https://github.com/mermaid-js/mermaid/releases

## Установка

Для автоматической установки выполните:

```bash
cd /home/user/Nmsdk/Rdk/GUI/Qt/static/markdown
curl -o marked.min.js https://cdn.jsdelivr.net/npm/marked/marked.min.js
curl -o mermaid.min.js https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js
```

Или скачайте файлы вручную и поместите их в эту папку.

## Примечание

Если библиотеки отсутствуют, виджет будет использовать упрощенный рендеринг без поддержки Mermaid диаграмм.

---

## EN

This folder should contain the following JavaScript libraries for Markdown rendering:

1. **marked.min.js** — library for converting Markdown to HTML
   - Download from: https://cdn.jsdelivr.net/npm/marked/marked.min.js
   - Or: https://github.com/markedjs/marked/releases

2. **mermaid.min.js** — library for rendering Mermaid diagrams
   - Download from: https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js
   - Or: https://github.com/mermaid-js/mermaid/releases

## Installation

For automatic installation, run:

```bash
cd /home/user/Nmsdk/Rdk/GUI/Qt/static/markdown
curl -o marked.min.js https://cdn.jsdelivr.net/npm/marked/marked.min.js
curl -o mermaid.min.js https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js
```

Or download the files manually and place them in this folder.

## Note

If the libraries are missing, the widget will use simplified rendering without Mermaid diagram support.
