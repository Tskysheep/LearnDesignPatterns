#include "Visitor.h"
#include <cctype>
#include <iomanip>

// ============================================================
// 具体元素实现
// ============================================================

// ---- TextElement ----
TextElement::TextElement(const std::string& text, const std::string& fontName,
                         int fontSize, bool bold, bool italic)
    : text_(text), fontName_(fontName), fontSize_(fontSize), bold_(bold), italic_(italic) {}

// 双重分派关键：accept 调用 visitor.visit(*this)
// 编译器根据 *this 的静态类型选择正确的 visit 重载
void TextElement::accept(DocumentVisitor& visitor) {
    visitor.visit(*this);
}

std::string TextElement::typeName() const { return "Text"; }
const std::string& TextElement::getText() const { return text_; }
void TextElement::setText(const std::string& text) { text_ = text; }
const std::string& TextElement::getFontName() const { return fontName_; }
int TextElement::getFontSize() const { return fontSize_; }
bool TextElement::isBold() const { return bold_; }
bool TextElement::isItalic() const { return italic_; }

// ---- ImageElement ----
ImageElement::ImageElement(const std::string& filePath, int width, int height,
                           const std::string& altText)
    : filePath_(filePath), width_(width), height_(height), altText_(altText) {}

void ImageElement::accept(DocumentVisitor& visitor) {
    visitor.visit(*this);
}

std::string ImageElement::typeName() const { return "Image"; }
const std::string& ImageElement::getFilePath() const { return filePath_; }
int ImageElement::getWidth() const { return width_; }
int ImageElement::getHeight() const { return height_; }
const std::string& ImageElement::getAltText() const { return altText_; }

std::string ImageElement::getFormat() const {
    auto pos = filePath_.rfind('.');
    if (pos != std::string::npos) {
        std::string ext = filePath_.substr(pos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
        return ext;
    }
    return "UNKNOWN";
}

// ---- TableElement ----
TableElement::TableElement(int rows, int cols, const std::string& caption)
    : rows_(rows), cols_(cols), caption_(caption),
      headers_(cols, ""),
      data_(rows, std::vector<std::string>(cols, "")) {}

void TableElement::accept(DocumentVisitor& visitor) {
    visitor.visit(*this);
}

std::string TableElement::typeName() const { return "Table"; }

void TableElement::setCell(int row, int col, const std::string& value) {
    if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
        data_[row][col] = value;
    }
}

std::string TableElement::getCell(int row, int col) const {
    if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
        return data_[row][col];
    }
    return "";
}

void TableElement::setHeader(int col, const std::string& header) {
    if (col >= 0 && col < cols_) {
        headers_[col] = header;
    }
}

std::string TableElement::getHeader(int col) const {
    if (col >= 0 && col < cols_) {
        return headers_[col];
    }
    return "";
}

int TableElement::getRows() const { return rows_; }
int TableElement::getCols() const { return cols_; }
const std::string& TableElement::getCaption() const { return caption_; }

// ============================================================
// 具体访问者实现
// 每个访问者定义了一种"横切"操作，作用于所有元素类型
// 新增访问者不需要修改任何 Element 类 — 开闭原则
// ============================================================

// ---- HtmlRenderVisitor ----
void HtmlRenderVisitor::visit(TextElement& element) {
    std::string tag = "p";
    if (element.getFontSize() >= 24) tag = "h1";
    else if (element.getFontSize() >= 18) tag = "h2";

    output_ << "<" << tag << " style=\"font-family:" << element.getFontName()
            << ";font-size:" << element.getFontSize() << "px;\">";

    if (element.isBold()) output_ << "<strong>";
    if (element.isItalic()) output_ << "<em>";
    output_ << element.getText();
    if (element.isItalic()) output_ << "</em>";
    if (element.isBold()) output_ << "</strong>";

    output_ << "</" << tag << ">\n";
}

void HtmlRenderVisitor::visit(ImageElement& element) {
    output_ << "<figure>\n";
    output_ << "  <img src=\"" << element.getFilePath()
            << "\" width=\"" << element.getWidth()
            << "\" height=\"" << element.getHeight()
            << "\" alt=\"" << element.getAltText() << "\" />\n";
    if (!element.getAltText().empty()) {
        output_ << "  <figcaption>" << element.getAltText() << "</figcaption>\n";
    }
    output_ << "</figure>\n";
}

void HtmlRenderVisitor::visit(TableElement& element) {
    output_ << "<table>\n";
    if (!element.getCaption().empty()) {
        output_ << "  <caption>" << element.getCaption() << "</caption>\n";
    }
    // 表头
    output_ << "  <thead><tr>\n";
    for (int c = 0; c < element.getCols(); ++c) {
        output_ << "    <th>" << element.getHeader(c) << "</th>\n";
    }
    output_ << "  </tr></thead>\n";
    // 数据行
    output_ << "  <tbody>\n";
    for (int r = 0; r < element.getRows(); ++r) {
        output_ << "  <tr>\n";
        for (int c = 0; c < element.getCols(); ++c) {
            output_ << "    <td>" << element.getCell(r, c) << "</td>\n";
        }
        output_ << "  </tr>\n";
    }
    output_ << "  </tbody>\n</table>\n";
}

std::string HtmlRenderVisitor::getResult() const { return output_.str(); }
void HtmlRenderVisitor::clear() { output_.str(""); output_.clear(); }

// ---- MarkdownExportVisitor ----
void MarkdownExportVisitor::visit(TextElement& element) {
    if (element.getFontSize() >= 24) {
        output_ << "# ";
    } else if (element.getFontSize() >= 18) {
        output_ << "## ";
    }

    if (element.isBold() && element.isItalic()) {
        output_ << "***" << element.getText() << "***";
    } else if (element.isBold()) {
        output_ << "**" << element.getText() << "**";
    } else if (element.isItalic()) {
        output_ << "*" << element.getText() << "*";
    } else {
        output_ << element.getText();
    }
    output_ << "\n\n";
}

void MarkdownExportVisitor::visit(ImageElement& element) {
    output_ << "![" << element.getAltText() << "](" << element.getFilePath() << ")\n\n";
}

void MarkdownExportVisitor::visit(TableElement& element) {
    if (!element.getCaption().empty()) {
        output_ << "**" << element.getCaption() << "**\n\n";
    }
    // 表头
    output_ << "| ";
    for (int c = 0; c < element.getCols(); ++c) {
        output_ << element.getHeader(c) << " | ";
    }
    output_ << "\n| ";
    for (int c = 0; c < element.getCols(); ++c) {
        output_ << "--- | ";
    }
    output_ << "\n";
    // 数据行
    for (int r = 0; r < element.getRows(); ++r) {
        output_ << "| ";
        for (int c = 0; c < element.getCols(); ++c) {
            output_ << element.getCell(r, c) << " | ";
        }
        output_ << "\n";
    }
    output_ << "\n";
}

std::string MarkdownExportVisitor::getResult() const { return output_.str(); }
void MarkdownExportVisitor::clear() { output_.str(""); output_.clear(); }

// ---- PlainTextExtractVisitor ----
void PlainTextExtractVisitor::visit(TextElement& element) {
    output_ << element.getText() << "\n";
}

void PlainTextExtractVisitor::visit(ImageElement& element) {
    if (!element.getAltText().empty()) {
        output_ << "[Image: " << element.getAltText() << "]\n";
    }
}

void PlainTextExtractVisitor::visit(TableElement& element) {
    for (int r = 0; r < element.getRows(); ++r) {
        for (int c = 0; c < element.getCols(); ++c) {
            std::string cell = element.getCell(r, c);
            if (!cell.empty()) {
                output_ << cell;
                if (c < element.getCols() - 1) output_ << "\t";
            }
        }
        output_ << "\n";
    }
}

std::string PlainTextExtractVisitor::getResult() const { return output_.str(); }
void PlainTextExtractVisitor::clear() { output_.str(""); output_.clear(); }

// ---- DocumentStatsVisitor ----
void DocumentStatsVisitor::visit(TextElement& element) {
    textCount_++;
    // 简单的单词计数：按空格分割
    std::istringstream iss(element.getText());
    std::string word;
    while (iss >> word) {
        totalWords_++;
    }
}

void DocumentStatsVisitor::visit(ImageElement& element) {
    imageCount_++;
    totalImagePixels_ += static_cast<long long>(element.getWidth()) * element.getHeight();
}

void DocumentStatsVisitor::visit(TableElement& element) {
    tableCount_++;
    totalCells_ += element.getRows() * element.getCols();
}

void DocumentStatsVisitor::printStats() const {
    std::cout << "  Document Statistics:\n";
    std::cout << "    Text elements:  " << textCount_ << " (" << totalWords_ << " words)\n";
    std::cout << "    Image elements: " << imageCount_
              << " (" << totalImagePixels_ << " total pixels)\n";
    std::cout << "    Table elements: " << tableCount_ << " (" << totalCells_ << " cells)\n";
    std::cout << "    Total elements: " << (textCount_ + imageCount_ + tableCount_) << "\n";
}

int DocumentStatsVisitor::getTextCount() const { return textCount_; }
int DocumentStatsVisitor::getImageCount() const { return imageCount_; }
int DocumentStatsVisitor::getTableCount() const { return tableCount_; }
int DocumentStatsVisitor::getTotalWords() const { return totalWords_; }
int DocumentStatsVisitor::getTotalCells() const { return totalCells_; }

// ---- SpellCheckVisitor ----
SpellCheckVisitor::SpellCheckVisitor() {
    // 简化的"字典"：常用英文单词列表
    knownWords_ = {
        "the", "a", "an", "is", "are", "was", "were", "be", "been",
        "and", "or", "but", "not", "no", "yes", "this", "that",
        "it", "its", "in", "on", "at", "to", "for", "of", "with",
        "by", "from", "as", "into", "through", "during", "before",
        "design", "pattern", "patterns", "software", "system", "document",
        "report", "quarterly", "sales", "revenue", "growth", "annual",
        "performance", "analysis", "data", "total", "market", "product",
        "results", "summary", "introduction", "conclusion", "overview",
        "table", "figure", "image", "text", "page", "section",
        "q1", "q2", "q3", "q4", "million", "percent", "year",
        "strong", "shows", "our", "key", "findings", "company",
        "has", "have", "had", "do", "does", "did", "will",
        "can", "could", "should", "would", "may", "might",
        "achieved", "significant", "compared", "previous",
        "financial", "metrics", "below", "above", "chart",
        "each", "every", "all", "some", "any", "most",
        "new", "old", "first", "last", "next", "same",
        "regional", "distribution"
    };
}

std::vector<std::string> SpellCheckVisitor::splitWords(const std::string& text) const {
    std::vector<std::string> words;
    std::string word;
    for (char ch : text) {
        if (std::isalpha(static_cast<unsigned char>(ch))) {
            word += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        } else {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty()) words.push_back(word);
    return words;
}

bool SpellCheckVisitor::isKnownWord(const std::string& word) const {
    return std::find(knownWords_.begin(), knownWords_.end(), word) != knownWords_.end();
}

void SpellCheckVisitor::visit(TextElement& element) {
    auto words = splitWords(element.getText());
    for (const auto& word : words) {
        if (!isKnownWord(word)) {
            errors_.emplace_back("Text: \"" + element.getText().substr(0, 40) + "...\"", word);
        }
    }
}

void SpellCheckVisitor::visit(ImageElement& element) {
    // 检查图片的 alt text
    if (!element.getAltText().empty()) {
        auto words = splitWords(element.getAltText());
        for (const auto& word : words) {
            if (!isKnownWord(word)) {
                errors_.emplace_back("Image alt: \"" + element.getAltText() + "\"", word);
            }
        }
    }
}

void SpellCheckVisitor::visit(TableElement& element) {
    // 检查表头和单元格内容
    for (int c = 0; c < element.getCols(); ++c) {
        auto words = splitWords(element.getHeader(c));
        for (const auto& word : words) {
            if (!isKnownWord(word)) {
                errors_.emplace_back("Table header", word);
            }
        }
    }
    for (int r = 0; r < element.getRows(); ++r) {
        for (int c = 0; c < element.getCols(); ++c) {
            auto words = splitWords(element.getCell(r, c));
            for (const auto& word : words) {
                if (!isKnownWord(word)) {
                    errors_.emplace_back(
                        "Table cell [" + std::to_string(r) + "," + std::to_string(c) + "]",
                        word);
                }
            }
        }
    }
}

void SpellCheckVisitor::printReport() const {
    std::cout << "  Spell Check Report:\n";
    if (errors_.empty()) {
        std::cout << "    No spelling errors found.\n";
    } else {
        std::cout << "    Found " << errors_.size() << " potential spelling issue(s):\n";
        for (const auto& [context, word] : errors_) {
            std::cout << "    - \"" << word << "\" in " << context << "\n";
        }
    }
}

int SpellCheckVisitor::getErrorCount() const {
    return static_cast<int>(errors_.size());
}

// ============================================================
// Document 容器实现
// ============================================================
Document::Document(const std::string& title) : title_(title) {}

void Document::addElement(std::unique_ptr<DocumentElement> element) {
    elements_.push_back(std::move(element));
}

// 遍历所有元素，让每个元素接受访问者
void Document::accept(DocumentVisitor& visitor) {
    for (auto& element : elements_) {
        element->accept(visitor);
    }
}

const std::string& Document::getTitle() const { return title_; }
size_t Document::elementCount() const { return elements_.size(); }

// ============================================================
// main() — 演示访问者模式的完整用法
// ============================================================
int main() {
    std::cout << "============================================\n";
    std::cout << " Visitor Pattern - Document Processing\n";
    std::cout << "============================================\n\n";

    // 构建文档 — 包含各种元素
    Document doc("Quarterly Sales Report");

    // 标题文本
    doc.addElement(std::make_unique<TextElement>(
        "Quarterly Sales Report - Q3 2025", "Arial", 24, true, false));

    // 正文段落
    doc.addElement(std::make_unique<TextElement>(
        "This report shows our quarterly sales performance analysis and key findings.",
        "Times New Roman", 12, false, false));

    // 强调文本
    doc.addElement(std::make_unique<TextElement>(
        "Revenue has achieved significent growth compared to previous year.",
        "Arial", 12, false, true));
    // 注意："significent" 是故意的拼写错误，用于测试 SpellCheckVisitor

    // 销售数据图表
    doc.addElement(std::make_unique<ImageElement>(
        "charts/sales_chart.png", 800, 600, "Quarterly sales chart"));

    // 区域分布图
    doc.addElement(std::make_unique<ImageElement>(
        "charts/regional_distribution.jpg", 640, 480, "Regional sales distribution"));

    // 销售数据表
    auto table = std::make_unique<TableElement>(3, 4, "Regional Sales Data (in millions)");
    table->setHeader(0, "Region");
    table->setHeader(1, "Q1");
    table->setHeader(2, "Q2");
    table->setHeader(3, "Q3");
    table->setCell(0, 0, "North America");
    table->setCell(0, 1, "12.5");
    table->setCell(0, 2, "14.2");
    table->setCell(0, 3, "16.8");
    table->setCell(1, 0, "Europe");
    table->setCell(1, 1, "8.3");
    table->setCell(1, 2, "9.1");
    table->setCell(1, 3, "11.4");
    table->setCell(2, 0, "Asia Pacific");
    table->setCell(2, 1, "6.7");
    table->setCell(2, 2, "7.9");
    table->setCell(2, 3, "9.2");
    doc.addElement(std::move(table));

    // 结论
    doc.addElement(std::make_unique<TextElement>(
        "Conclusion: Strong performance across all regions with total revenue growth of 18 percent.",
        "Arial", 12, true, false));

    std::cout << "Document: \"" << doc.getTitle() << "\" (" << doc.elementCount() << " elements)\n\n";

    // --- 1. HTML 渲染 ---
    std::cout << "=== 1. HTML Render Visitor ===\n";
    HtmlRenderVisitor htmlVisitor;
    doc.accept(htmlVisitor);
    std::cout << htmlVisitor.getResult() << "\n";

    // --- 2. Markdown 导出 ---
    std::cout << "=== 2. Markdown Export Visitor ===\n";
    MarkdownExportVisitor mdVisitor;
    doc.accept(mdVisitor);
    std::cout << mdVisitor.getResult() << "\n";

    // --- 3. 纯文本提取 ---
    std::cout << "=== 3. Plain Text Extract Visitor ===\n";
    PlainTextExtractVisitor textVisitor;
    doc.accept(textVisitor);
    std::cout << textVisitor.getResult() << "\n";

    // --- 4. 文档统计 ---
    std::cout << "=== 4. Document Stats Visitor ===\n";
    DocumentStatsVisitor statsVisitor;
    doc.accept(statsVisitor);
    statsVisitor.printStats();
    std::cout << "\n";

    // --- 5. 拼写检查 ---
    std::cout << "=== 5. Spell Check Visitor ===\n";
    SpellCheckVisitor spellVisitor;
    doc.accept(spellVisitor);
    spellVisitor.printReport();

    std::cout << "\n============================================\n";
    std::cout << " Visitor Pattern Demo Complete\n";
    std::cout << "============================================\n";

    return 0;
}
