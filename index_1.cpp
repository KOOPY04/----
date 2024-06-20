#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

// �w�q�O�����c
struct Record {
    string student_id;  // �ǥ�ID
    string course_id;   // �ҵ{ID
    string course_name; // �ҵ{�W��
};

// �w�q���ޱ��ص��c
struct IndexEntry {
    string key;  // ������ȡ]�ǥ�ID�νҵ{ID�^
    string data; // ������ơ]�ҵ{ID�ξǥ�ID�^
    long offset; // �b����ɮפ��������q
};

// �w�q�G���j�M�𵲺c
struct TreeNode {
    IndexEntry entry;
    TreeNode* left;
    TreeNode* right;

    TreeNode(IndexEntry e) : entry(e), left(nullptr), right(nullptr) {}
};

// ���J�G���j�M��`�I
TreeNode* insert(TreeNode* root, IndexEntry entry) {
    if (root == nullptr) {
        return new TreeNode(entry);
    }
    if (entry.key < root->entry.key) {
        root->left = insert(root->left, entry);
    } else {
        root->right = insert(root->right, entry);
    }
    return root;
}

// �N�G���j�M��g�J���ޤ��
void writeTreeToFile(TreeNode* root, ofstream& outFile) {
    if (root == nullptr) {
        return;
    }
    outFile << root->entry.key << " " << root->entry.data << " " << root->entry.offset << '\n';
    writeTreeToFile(root->left, outFile);
    writeTreeToFile(root->right, outFile);
    delete root; // ����`�I
}

// �ھھǥ�ID�إ߯���
void createStudentIndex(const vector<string>& dataFiles, const string& indexFile) {
    ofstream indexFp(indexFile);

    if (!indexFp.is_open()) {
        cerr << "�L�k���}���ޤ��" << endl;
        exit(EXIT_FAILURE);
    }

    TreeNode* root = nullptr;

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "�L�k���}�ƾڤ��: " << dataFile << endl;
            continue;
        }

        string line;
        long offset = 0;
        IndexEntry entry;

        // �v��Ū������ɮ�
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // ���L�Ĥ@��

            entry.key = record.student_id;
            entry.data = record.course_id;
            entry.offset = offset;
            root = insert(root, entry);

            offset += line.length() + 1; // ��s�����q�]�]�t����š^
        }

        dataFp.close();
    }

    writeTreeToFile(root, indexFp);
    indexFp.close();
}

// �ھڽҵ{ID�إ߯���
void createCourseIndex(const vector<string>& dataFiles, const string& indexFile) {
    ofstream indexFp(indexFile);

    if (!indexFp.is_open()) {
        cerr << "�L�k���}���ޤ��" << endl;
        exit(EXIT_FAILURE);
    }

    unordered_map<string, long> courseIndex; // �ҵ{ID -> �����q

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "�L�k���}�ƾڤ��: " << dataFile << endl;
            continue;
        }

        string line;
        long offset = 0;

        // �v��Ū������ɮ�
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // ���L�Ĥ@��

            courseIndex[record.course_id] = offset;

            offset += line.length() + 1; // ��s�����q�]�]�t����š^
        }

        dataFp.close();
    }

    // �ҵ{ID���޼g�J���
    for (const auto& entry : courseIndex) {
        indexFp << entry.first << " " << entry.second << '\n';
    }

    indexFp.close();
}

// �c�y�ƾڤ��W�٦C��
vector<string> generateDataFileNames(int fileCount) {
    vector<string> dataFiles;
    for (int i = 1; i <= fileCount; i++) {
        stringstream fileNo;
        if (i < 10) {
            fileNo << "data_big5/000" << i;
        } else if (i < 100) {
            fileNo << "data_big5/00" << i;
        } else {
            fileNo << "data_big5/0" << i;
        }
        dataFiles.push_back(fileNo.str());
    }
    return dataFiles;
}

int main() {
    // Ū���h���ɮ�
    vector<string> dataFiles = generateDataFileNames(466);
    const string studentIndexFile = "student_index.txt"; // �ǥ�ID�����ɮצW��
    const string courseIndexFile = "course_index.txt";   // �ҵ{ID�����ɮצW��

    // �إ߾ǥ�ID����
    createStudentIndex(dataFiles, studentIndexFile);
    cout << "�ǥ�ID���ޫإߧ���" << endl;

    // �إ߽ҵ{ID����
    createCourseIndex(dataFiles, courseIndexFile);
    cout << "�ҵ{ID���ޫإߧ���" << endl;

    return 0;
}
