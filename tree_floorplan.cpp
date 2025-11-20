#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

struct Node { //定義 B*-tree 中每一個節點的資料結構，包括左子、右子、父節點指標
    std::string name;
    int width, height;
    int x = 0, y = 0;
    Node *parent = nullptr, *left = nullptr, *right = nullptr;

    Node(std::string n, int w, int h) : name(n), width(w), height(h) {}
};

struct Interval {
    int left, right, y;
    Interval(int l, int r, int h) : left(l), right(r), y(h) {}
};

std::map<std::string, Node*> nodes;
std::vector<std::string> input_order;
std::vector<Interval> contour;

int find_y(int x, int width) { //查找當前區段 contour 上最高點 → 决定 node 的 y
    int maxY = 0;
    for (auto& c : contour) {
        if (c.right <= x || c.left >= x + width) continue;
        maxY = std::max(maxY, c.y);
    }
    return maxY;
}

void update_contour(int x, int width, int top_y) { //將這個 node 加到 contour 上（更新最高輪廓線）
    int x_end = x + width;
    std::vector<Interval> newC;
    for (const auto& c : contour) {
        if (c.right <= x || c.left >= x_end) {
            newC.push_back(c);
        } else {
            if (c.left < x) newC.emplace_back(c.left, x, c.y);
            if (c.right > x_end) newC.emplace_back(x_end, c.right, c.y);
        }
    }
    newC.emplace_back(x, x_end, top_y);
    std::sort(newC.begin(), newC.end(), [](const Interval& a, const Interval& b) {
        return a.left < b.left;
    });
    contour = newC;
}

void traversal(Node* node) { //遞迴遍歷 B*-tree
    if (!node) return;
    if (node->parent == nullptr) node->x = 0; //根據 parent 計算 x 位置
    else if (node->parent->left == node) node->x = node->parent->x + node->parent->width;
    else node->x = node->parent->x;

    node->y = find_y(node->x, node->width);
    update_contour(node->x, node->width, node->y + node->height); //根據 contour 計算 y

    traversal(node->left);
    traversal(node->right);
}

void readNodes(const std::string& filename, Node*& root) {
    std::ifstream fin(filename);
    std::string line;
    std::map<std::string, std::string> pMap, lMap, rMap;

    std::getline(fin, line); // Skip NumNode

    while (std::getline(fin, line)) {
        if (line.find("name") == std::string::npos) continue;
        std::string name = line.substr(line.find(":") + 1);
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t\r\n") + 1);

        std::getline(fin, line); // dimension
        std::istringstream iss_dim(line.substr(line.find(":") + 1));
        int w, h;
        iss_dim >> w >> h;

        std::getline(fin, line); // relation
        std::istringstream iss_rel(line.substr(line.find(":") + 1));
        std::string parent, left, right;
        iss_rel >> parent >> left >> right;

        parent.erase(parent.find_last_not_of(" \t\r\n") + 1);
        left.erase(left.find_last_not_of(" \t\r\n") + 1);
        right.erase(right.find_last_not_of(" \t\r\n") + 1);

        Node* node = new Node(name, w, h);
        nodes[name] = node;
        input_order.push_back(name);
        //使用 map 暫存 parent/left/right
        pMap[name] = parent;
        lMap[name] = left;
        rMap[name] = right;
    }

    for (const std::string& name : input_order) {
        Node* node = nodes[name];
        if (pMap[name] != "X" && nodes.count(pMap[name]))
            node->parent = nodes[pMap[name]];
        if (lMap[name] != "X" && nodes.count(lMap[name]))
            node->left = nodes[lMap[name]];
        if (rMap[name] != "X" && nodes.count(rMap[name]))
            node->right = nodes[rMap[name]];
    }

    for (const auto& it : pMap) {
        const std::string& name = it.first;
        const std::string& parent = it.second;
        if (parent == "X") {
            root = nodes[name];
            break;
        }
    }
    
}

void applyPerturbation(std::ifstream& fin) {
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string op, colon, name1, name2;
        iss >> op >> colon >> name1;

        if (op == "rotate" && nodes.count(name1)) { //若是 rotate：交換該模組寬高
            std::swap(nodes[name1]->width, nodes[name1]->height);
            std::cout << "Rotated: " << name1 << "\n";
        } else if (op == "swap") { //若是 swap：交換兩個模組的 name / width / height / node 實體
            iss >> name2;
            if (nodes.count(name1) && nodes.count(name2)) {
                std::swap(nodes[name1]->width, nodes[name2]->width);
                std::swap(nodes[name1]->height, nodes[name2]->height);
                std::swap(nodes[name1]->name, nodes[name2]->name);
                std::swap(nodes[name1], nodes[name2]);
                std::cout << "Swapped: " << name1 << " <--> " << name2 << "\n";
            }
        }
    }
}

void outputTXT(const std::string& filename) { //name 與 lowerleft 位置
    std::ofstream fout(filename);
    for (const auto& name : input_order) {
        Node* n = nodes[name];
        fout << "\nname : " << n->name << "\n";
        fout << "lowerleft : " << n->x << " " << n->y << "\n";
    }
}

void outputMATLAB(const std::string& filename) { //產生 MATLAB 可視化繪圖程式碼
    std::ofstream fout(filename);
    fout << "axis equal;\nhold on;\ngrid on;\n\n";
    for (const auto& name : input_order) {
        Node* node = nodes[name];
        int x = node->x, y = node->y, w = node->width, h = node->height;
        fout << "block_x=[ " << x << " " << x+w << " " << x+w << " " << x << " " << x << "];\n";
        fout << "block_y=[ " << y << " " << y << " " << y+h << " " << y+h << " " << y << "];\n";
        fout << "fill(block_x,block_y,'c');\n";
        fout << "text(" << (x + w/2.0) << "," << (y + h/2.0) << ", '" << name
             << "', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle');\n\n";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./HW2_學號 benchmark_name\n";
        return 1;
    }

    std::string benchmark = argv[1];
    std::string folder = "Benchmark/" + benchmark + "/";
    std::string nodeFile = folder + benchmark + ".nodes";
    std::string pertFile = folder + benchmark + ".perturbation";
    std::string txtFile = folder + benchmark + ".txt";
    std::string mFile = folder + benchmark + ".m";

    Node* root = nullptr;
    readNodes(nodeFile, root); // 讀取 .nodes 檔案並建立 B*-tree 結構

    std::ifstream finPert(pertFile); // 根據 .perturbation 做 rotate / swap 操作
    if (finPert.is_open()) {
        applyPerturbation(finPert);
        finPert.close();
    }

    traversal(root); // 走訪 B*-tree 並計算每個模組的 x/y
    outputTXT(txtFile); // 輸出成 txt 格式
    outputMATLAB(mFile); // 輸出成 MATLAB 畫圖程式碼

    return 0;
}
