#include <bits/stdc++.h>
using namespace std;

class Node {
public:
    string name;
    time_t createdAt;

    Node(string name) {
        this->name = name;
        createdAt = time(nullptr);
    }

    virtual ~Node() {}
    virtual bool isDirectory() = 0;
};

class File : public Node {
public:
    string content;
    string type;

    File(string name, string type = "txt")
        : Node(name) {
        this->type = type;
    }

    bool isDirectory() override {
        return false;
    }
};

class Directory : public Node {
public:
    unordered_map<string, Node*> children;

    Directory(string name) : Node(name) {}

    bool isDirectory() override {
        return true;
    }

    ~Directory() {
        for (auto &it : children)
            delete it.second;
    }
};

class FileSystem {
private:
    Directory* root;

    vector<string> split(string path) {
        vector<string> parts;
        string temp;

        for (char c : path) {
            if (c == '/') {
                if (!temp.empty()) {
                    parts.push_back(temp);
                    temp.clear();
                }
            } else {
                temp += c;
            }
        }

        if (!temp.empty())
            parts.push_back(temp);

        return parts;
    }

    Node* getNode(string path) {
        if (path == "/")
            return root;

        vector<string> parts = split(path);
        Node* current = root;

        for (string part : parts) {
            if (!current->isDirectory())
                return nullptr;

            Directory* dir = (Directory*)current;

            if (!dir->children.count(part))
                return nullptr;

            current = dir->children[part];
        }

        return current;
    }

    pair<Directory*, string> getParent(string path) {
        vector<string> parts = split(path);

        if (parts.empty())
            return {nullptr, ""};

        string name = parts.back();
        parts.pop_back();

        Directory* current = root;

        for (string part : parts) {
            if (!current->children.count(part))
                return {nullptr, ""};

            Node* node = current->children[part];

            if (!node->isDirectory())
                return {nullptr, ""};

            current = (Directory*)node;
        }

        return {current, name};
    }

public:
    FileSystem() {
        root = new Directory("/");
    }

    ~FileSystem() {
        delete root;
    }

    bool createDirectory(string path) {
        auto [parent, name] = getParent(path);

        if (!parent || parent->children.count(name))
            return false;

        parent->children[name] = new Directory(name);
        return true;
    }

    bool createFile(string path, string content,
                    string type = "txt") {
        auto [parent, name] = getParent(path);

        if (!parent || parent->children.count(name))
            return false;

        File* file = new File(name, type);
        file->content = content;

        parent->children[name] = file;
        return true;
    }

    string readFile(string path) {
        Node* node = getNode(path);

        if (!node || node->isDirectory())
            return "File not found";

        File* file = (File*)node;
        return file->content;
    }

    bool updateFile(string path, string content) {
        Node* node = getNode(path);

        if (!node || node->isDirectory())
            return false;

        File* file = (File*)node;
        file->content = content;

        return true;
    }

    bool deleteNode(string path) {
        auto [parent, name] = getParent(path);

        if (!parent || !parent->children.count(name))
            return false;

        delete parent->children[name];
        parent->children.erase(name);

        return true;
    }

    void listDirectory(string path) {
        Node* node = getNode(path);

        if (!node || !node->isDirectory())
            return;

        Directory* dir = (Directory*)node;

        for (auto &[name, child] : dir->children) {
            cout << name;

            if (child->isDirectory())
                cout << "/";

            cout << endl;
        }
    }
};

int main() {
    FileSystem fs;

    fs.createDirectory("/documents");
    fs.createDirectory("/documents/projects");

    fs.createFile(
        "/documents/report.txt",
        "This is my report"
    );

    fs.createFile(
        "/documents/projects/code.cpp",
        "cout << \"Hello\";",
        "cpp"
    );

    cout << fs.readFile("/documents/report.txt") << endl;

    fs.updateFile(
        "/documents/report.txt",
        "Updated report"
    );

    cout << fs.readFile("/documents/report.txt") << endl;

    cout << "\nDocuments:\n";
    fs.listDirectory("/documents");

    fs.deleteNode("/documents/report.txt");

    return 0;
}

// output:
// This is my report
// Updated report

// Documents:
// report.txt
// projects/