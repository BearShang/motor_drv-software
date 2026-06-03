(function () {
  function slugify(text, index) {
    var base = text
      .trim()
      .toLowerCase()
      .replace(/\s+/g, "-")
      .replace(/[^\p{L}\p{N}_-]+/gu, "")
      .replace(/^-+|-+$/g, "");
    return base || "section-" + index;
  }

  function buildToc() {
    var article = document.querySelector(".doc-article");
    var toc = document.getElementById("page-toc");
    if (!article || !toc) return;

    var headings = Array.prototype.slice.call(article.querySelectorAll("h2, h3"));
    if (!headings.length) {
      toc.innerHTML = '<span class="toc-empty">暂无章节</span>';
      return;
    }

    var usedIds = Object.create(null);
    var links = headings.map(function (heading, index) {
      if (!heading.id) {
        var id = slugify(heading.textContent, index + 1);
        var unique = id;
        var counter = 2;
        while (usedIds[unique] || document.getElementById(unique)) {
          unique = id + "-" + counter;
          counter += 1;
        }
        heading.id = unique;
      }
      usedIds[heading.id] = true;

      var link = document.createElement("a");
      link.href = "#" + heading.id;
      link.textContent = heading.textContent;
      link.dataset.level = heading.tagName === "H3" ? "3" : "2";
      return link;
    });

    links.forEach(function (link) {
      toc.appendChild(link);
    });

    if ("IntersectionObserver" in window) {
      var activeId = "";
      var observer = new IntersectionObserver(
        function (entries) {
          entries.forEach(function (entry) {
            if (entry.isIntersecting) {
              activeId = entry.target.id;
            }
          });
          links.forEach(function (link) {
            link.classList.toggle("is-active", link.getAttribute("href") === "#" + activeId);
          });
        },
        {
          rootMargin: "-20% 0px -70% 0px",
          threshold: 0
        }
      );
      headings.forEach(function (heading) {
        observer.observe(heading);
      });
    }
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", buildToc);
  } else {
    buildToc();
  }
})();
