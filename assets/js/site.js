(function () {
  function onReady(callback) {
    if (document.readyState === "loading") {
      document.addEventListener("DOMContentLoaded", callback);
    } else {
      callback();
    }
  }

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

  function normalize(text) {
    return (text || "").toString().toLowerCase().replace(/\s+/g, " ").trim();
  }

  function getSnippet(content, query) {
    var source = (content || "").replace(/\s+/g, " ").trim();
    if (!source) return "";

    var normalizedSource = source.toLowerCase();
    var normalizedQuery = normalize(query);
    var position = normalizedSource.indexOf(normalizedQuery);
    if (position < 0) {
      var firstTerm = normalizedQuery.split(" ")[0];
      position = firstTerm ? normalizedSource.indexOf(firstTerm) : 0;
    }
    if (position < 0) position = 0;

    var start = Math.max(0, position - 42);
    var end = Math.min(source.length, position + normalizedQuery.length + 86);
    var prefix = start > 0 ? "..." : "";
    var suffix = end < source.length ? "..." : "";
    return prefix + source.slice(start, end) + suffix;
  }

  function scorePage(page, query, terms) {
    var title = page.searchTitle;
    var content = page.searchContent;
    var hasAllTerms = terms.every(function (term) {
      return title.indexOf(term) >= 0 || content.indexOf(term) >= 0;
    });
    if (!hasAllTerms) return 0;

    var score = 1;
    if (title === query) score += 120;
    if (title.indexOf(query) >= 0) score += 70;
    if (content.indexOf(query) >= 0) score += 20;

    terms.forEach(function (term) {
      if (title.indexOf(term) >= 0) score += 18;
      if (content.indexOf(term) >= 0) score += 4;
    });

    return score;
  }

  function setupSearch() {
    var form = document.querySelector(".site-search");
    var input = document.getElementById("site-search-input");
    var results = document.getElementById("site-search-results");
    if (!form || !input || !results) return;

    var indexUrl = form.getAttribute("data-search-index");
    var pages = [];
    var activeIndex = -1;

    function closeResults() {
      results.classList.remove("is-open");
      results.innerHTML = "";
      activeIndex = -1;
    }

    function setActive(index) {
      var links = Array.prototype.slice.call(results.querySelectorAll(".search-result"));
      activeIndex = Math.max(-1, Math.min(index, links.length - 1));
      links.forEach(function (link, linkIndex) {
        link.classList.toggle("is-active", linkIndex === activeIndex);
      });
    }

    function renderResults(matches, query) {
      results.innerHTML = "";

      if (!query) {
        closeResults();
        return;
      }

      if (!matches.length) {
        var empty = document.createElement("div");
        empty.className = "search-empty";
        empty.textContent = "未找到相关文档";
        results.appendChild(empty);
        results.classList.add("is-open");
        return;
      }

      matches.slice(0, 6).forEach(function (page) {
        var link = document.createElement("a");
        link.className = "search-result";
        link.href = page.url;
        link.setAttribute("role", "option");

        var title = document.createElement("div");
        title.className = "search-result-title";
        title.textContent = page.title;

        var snippet = document.createElement("div");
        snippet.className = "search-result-snippet";
        snippet.textContent = getSnippet(page.content, query);

        link.appendChild(title);
        link.appendChild(snippet);
        results.appendChild(link);
      });

      results.classList.add("is-open");
      setActive(-1);
    }

    function search(query) {
      var normalizedQuery = normalize(query);
      if (!normalizedQuery) {
        closeResults();
        return;
      }

      var terms = normalizedQuery.split(" ").filter(Boolean);
      var matches = pages
        .map(function (page) {
          return {
            page: page,
            score: scorePage(page, normalizedQuery, terms)
          };
        })
        .filter(function (entry) {
          return entry.score > 0;
        })
        .sort(function (a, b) {
          return b.score - a.score;
        })
        .map(function (entry) {
          return entry.page;
        });

      renderResults(matches, normalizedQuery);
    }

    fetch(indexUrl)
      .then(function (response) {
        if (!response.ok) throw new Error("Search index unavailable");
        return response.json();
      })
      .then(function (data) {
        pages = data.map(function (page) {
          return {
            title: page.title || "",
            url: page.url || "#",
            content: page.content || "",
            searchTitle: normalize(page.title),
            searchContent: normalize(page.content)
          };
        });
        if (document.activeElement === input && input.value) {
          search(input.value);
        }
      })
      .catch(function () {
        input.disabled = true;
        input.placeholder = "搜索暂不可用";
      });

    input.addEventListener("input", function () {
      search(input.value);
    });

    input.addEventListener("focus", function () {
      search(input.value);
    });

    input.addEventListener("keydown", function (event) {
      var links = Array.prototype.slice.call(results.querySelectorAll(".search-result"));
      if (event.key === "Escape") {
        closeResults();
        input.blur();
      } else if (event.key === "ArrowDown" && links.length) {
        event.preventDefault();
        setActive(activeIndex + 1);
      } else if (event.key === "ArrowUp" && links.length) {
        event.preventDefault();
        setActive(activeIndex <= 0 ? links.length - 1 : activeIndex - 1);
      } else if (event.key === "Enter" && links.length) {
        event.preventDefault();
        window.location.href = links[Math.max(activeIndex, 0)].href;
      }
    });

    document.addEventListener("click", function (event) {
      if (!form.contains(event.target)) {
        closeResults();
      }
    });
  }

  function setupImageLightbox() {
    var article = document.querySelector(".doc-article");
    if (!article) return;

    var images = Array.prototype.slice.call(article.querySelectorAll("img"));
    if (!images.length) return;

    var overlay = document.createElement("div");
    overlay.className = "image-lightbox";
    overlay.setAttribute("role", "dialog");
    overlay.setAttribute("aria-modal", "true");
    overlay.setAttribute("aria-label", "Image preview");
    overlay.innerHTML =
      '<button class="image-lightbox__close" type="button" aria-label="Close image preview">×</button>' +
      '<img class="image-lightbox__image" alt="">' +
      '<div class="image-lightbox__caption"></div>';
    document.body.appendChild(overlay);

    var lightboxImage = overlay.querySelector(".image-lightbox__image");
    var caption = overlay.querySelector(".image-lightbox__caption");
    var closeButton = overlay.querySelector(".image-lightbox__close");
    var zoom = 1;
    var panX = 0;
    var panY = 0;
    var dragStartX = 0;
    var dragStartY = 0;
    var startPanX = 0;
    var startPanY = 0;
    var isDragging = false;

    function clamp(value, min, max) {
      return Math.min(Math.max(value, min), max);
    }

    function resetPan() {
      panX = 0;
      panY = 0;
      isDragging = false;
      lightboxImage.classList.remove("is-dragging");
    }

    function applyZoom() {
      if (zoom <= 1.01) {
        resetPan();
      }
      lightboxImage.style.transform =
        "translate(" + panX.toFixed(1) + "px, " + panY.toFixed(1) + "px) scale(" + zoom.toFixed(3) + ")";
      lightboxImage.classList.toggle("is-zoomed", zoom > 1.01);
    }

    function openLightbox(image) {
      zoom = 1;
      resetPan();
      lightboxImage.src = image.currentSrc || image.src;
      lightboxImage.alt = image.alt || "";
      applyZoom();
      caption.textContent = image.alt || "";
      caption.hidden = !image.alt;
      overlay.classList.add("is-open");
      document.body.classList.add("has-lightbox");
      closeButton.focus();
    }

    function closeLightbox() {
      overlay.classList.remove("is-open");
      document.body.classList.remove("has-lightbox");
      lightboxImage.removeAttribute("src");
      lightboxImage.removeAttribute("style");
      zoom = 1;
      resetPan();
    }

    images.forEach(function (image) {
      image.classList.add("is-zoomable");
      image.setAttribute("tabindex", "0");
      image.setAttribute("role", "button");
      image.setAttribute("aria-label", (image.alt || "Image") + " - click to enlarge");

      image.addEventListener("click", function () {
        openLightbox(image);
      });

      image.addEventListener("keydown", function (event) {
        if (event.key === "Enter" || event.key === " ") {
          event.preventDefault();
          openLightbox(image);
        }
      });
    });

    closeButton.addEventListener("click", closeLightbox);

    overlay.addEventListener("click", function (event) {
      if (event.target === overlay) {
        closeLightbox();
      }
    });

    overlay.addEventListener(
      "wheel",
      function (event) {
        if (!overlay.classList.contains("is-open")) return;
        event.preventDefault();
        var direction = event.deltaY < 0 ? 1 : -1;
        var factor = direction > 0 ? 1.12 : 1 / 1.12;
        zoom = clamp(zoom * factor, 0.5, 4);
        applyZoom();
      },
      { passive: false }
    );

    lightboxImage.addEventListener("pointerdown", function (event) {
      if (zoom <= 1.01) return;
      event.preventDefault();
      isDragging = true;
      dragStartX = event.clientX;
      dragStartY = event.clientY;
      startPanX = panX;
      startPanY = panY;
      lightboxImage.classList.add("is-dragging");
      if (lightboxImage.setPointerCapture) {
        lightboxImage.setPointerCapture(event.pointerId);
      }
    });

    lightboxImage.addEventListener("pointermove", function (event) {
      if (!isDragging) return;
      event.preventDefault();
      panX = startPanX + event.clientX - dragStartX;
      panY = startPanY + event.clientY - dragStartY;
      applyZoom();
    });

    function endDrag(event) {
      if (!isDragging) return;
      isDragging = false;
      lightboxImage.classList.remove("is-dragging");
      if (event && lightboxImage.releasePointerCapture) {
        lightboxImage.releasePointerCapture(event.pointerId);
      }
    }

    lightboxImage.addEventListener("pointerup", endDrag);
    lightboxImage.addEventListener("pointercancel", endDrag);
    lightboxImage.addEventListener("lostpointercapture", endDrag);

    document.addEventListener("keydown", function (event) {
      if (event.key === "Escape" && overlay.classList.contains("is-open")) {
        closeLightbox();
      }
    });
  }

  onReady(function () {
    buildToc();
    setupSearch();
    setupImageLightbox();
  });
})();
