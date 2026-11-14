const target = new URL(document.currentScript.dataset.target);
target.search = window.location.search;
target.hash = window.location.hash;
window.location.replace(target.href);
