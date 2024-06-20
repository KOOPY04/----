#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <direct.h> 

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

// �w�q�ҵ{�W�r���޵��c
struct CourseNameIndexEntry {
    string course_id;   // �ҵ{ID
    string course_name; // �ҵ{�W�r
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
void writeTreeToFile(TreeNode* root, ofstream& outFile, int& fileCount, string& indexFilePathCourse) {
    if (root == nullptr) {
        return;
    }
    const int maxFileSize = 2010; // �ؼФ��j�p����]2KB�^

    // �p��n�g�J����e�`�I��ƪ��w���j�p
    long long int currentSize = static_cast<long long int>(outFile.tellp()) + root->entry.key.length() + root->entry.data.length() + 2;


    // �p�G�W�L�ؼФ��j�p�A������e���A���}�s���
    if (currentSize >= maxFileSize) {
        outFile.close();
        fileCount++;
        outFile.open(indexFilePathCourse + "_" + to_string(fileCount) + ".txt");
        if (!outFile.is_open()) {
            cerr << "�L�k���}���ޤ��" << endl;
            exit(EXIT_FAILURE);
        }
        currentSize = static_cast<long long int>(outFile.tellp()) + root->entry.key.length() + root->entry.data.length() + 2;
    }

    outFile << root->entry.key << " " << root->entry.data << '\n';
    writeTreeToFile(root->left, outFile, fileCount, indexFilePathCourse);
    writeTreeToFile(root->right, outFile, fileCount, indexFilePathCourse);
    delete root; // ����`�I
}

// �ھھǥ�ID�إ߯���
void createStudentIndex(const vector<string>& dataFiles, const string& indexDir) {
    string indexFilePathStudent = indexDir + "/student_index";
    if (_mkdir(indexDir.c_str()) == -1) {
        cerr << "�L�k�Ыؤ��: " << indexDir << endl;
        exit(EXIT_FAILURE);
    }
    ofstream indexFp(indexFilePathStudent + "_1.txt");

    if (!indexFp.is_open()) {
        cerr << "�L�k���}���ޤ��" << endl;
        exit(EXIT_FAILURE);
    }

    TreeNode* root = nullptr;
    int fileCount = 1;
    

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

    writeTreeToFile(root, indexFp, fileCount, indexFilePathStudent);
    indexFp.close();
}

// �ھڽҵ{ID�إ߯���
void createCourseIndex(const vector<string>& dataFiles, const string& indexDir) {
    unordered_map<string, vector<string>> courseIndex; // �ҵ{ID -> �ǥ�ID�C��

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "�L�k���}�ƾڤ��: " << dataFile << endl;
            continue;
        }

        string line;
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // ���L�Ĥ@��

            courseIndex[record.course_id].push_back(record.student_id);
        }

        dataFp.close();
    }

    string indexFilePathCourse = indexDir + "/course_index";
    #ifdef _WIN32
        _mkdir(indexDir.c_str());
    #else 
        mkdir(indexDir.c_str(), 0777);
    #endif
    ofstream indexFp(indexFilePathCourse + "_1.txt");
    if (!indexFp.is_open()) {
        cerr << "�L�k���}���ޤ��" << endl;
        exit(EXIT_FAILURE);
    }

    // �إߨC�ӽҵ{ID������
    const int maxFileSize = 2010; // �ؼФ��j�p����]2KB�^
    long long currentSize = 0;
    int fileCount = 1;

    for (const auto& entry : courseIndex) {
        string header = entry.first + "\n";
        currentSize += header.length();

        if (currentSize >= maxFileSize) {
            indexFp.close();
            fileCount++;
            indexFp.open(indexFilePathCourse + "_" + to_string(fileCount) + ".txt");
            if (!indexFp.is_open()) {
                cerr << "�L�k���}���ޤ��" << endl;
                exit(EXIT_FAILURE);
            }
            currentSize = header.length();
            indexFp << header;
        } else {
            indexFp << header;
        }

        for (const auto& studentId : entry.second) {
            string line = studentId + "\n";
            currentSize += line.length();

            if (currentSize >= maxFileSize) {
                indexFp.close();
                fileCount++;
                indexFp.open(indexFilePathCourse + "_" + to_string(fileCount) + ".txt");
                if (!indexFp.is_open()) {
                    cerr << "�L�k���}���ޤ��" << endl;
                    exit(EXIT_FAILURE);
                }
                currentSize = header.length() + line.length();
                indexFp << header;
            }

            indexFp << line;
        }
    }

    indexFp.close();
}

// �إ߽ҵ{�W�r����
void createCourseNameIndex(const vector<string>& dataFiles, const string& indexDir) {

    unordered_map<string, CourseNameIndexEntry> courseNameIndex; // �ҵ{ID -> �ҵ{�W�r

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "�L�k���}�ƾڤ��: " << dataFile << endl;
            continue;
        }

        string line;

        // �v��Ū������ɮ�
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // ���L�Ĥ@��

            // �إ߽ҵ{�W�r����
            CourseNameIndexEntry entry;
            entry.course_id = record.course_id;
            entry.course_name = record.course_name;
            courseNameIndex[record.course_id] = entry;
        }

        dataFp.close();
    }


    string indexFilePathCourseName = indexDir + "/courseName_index";
    if (_mkdir(indexDir.c_str()) == -1) {
        cerr << "�L�k�Ыؤ��: " << indexDir << endl;
        exit(EXIT_FAILURE);
    }
    ofstream courseNameIndexFp;
    // �N�ҵ{�W�r���޼g�J���
    int fileCount = 1;
    const int maxFileSize = 2010; // �ؼФ��j�p����]2KB�^
    long long int currentSize = 0;

    courseNameIndexFp.open(indexFilePathCourseName + "_" + to_string(fileCount) + ".txt");
    if (!courseNameIndexFp.is_open()) {
        cerr << "�L�k���}���ޤ��" << endl;
        exit(EXIT_FAILURE);
    }

    // �N�ҵ{�W�r���޼g�J���
    for (const auto& entry : courseNameIndex) {
        // �p��n�g�J����e�O�����j�p
        long long int recordSize = entry.second.course_id.length() + entry.second.course_name.length() + 2;
    
        // �p�G�W�L�ؼФ��j�p�A������e���A���}�s���
        if (currentSize + recordSize >= maxFileSize) {
            courseNameIndexFp.close();
            fileCount++;
            courseNameIndexFp.open(indexFilePathCourseName + "_" + to_string(fileCount) + ".txt");
            if (!courseNameIndexFp.is_open()) {
                cerr << "�L�k���}���ޤ��" << endl;
                exit(EXIT_FAILURE);
            }
            currentSize = 0; // ���m��e���j�p�p�ƾ�
        }

        courseNameIndexFp << entry.second.course_id << " " << entry.second.course_name << '\n';
        currentSize += recordSize; // ��s��e���j�p�p�ƾ�
    }

    courseNameIndexFp.close();

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

    // �إ߾ǥ�ID����
    createStudentIndex(dataFiles, "student_id_index");
    cout << "�ǥ�ID���ޫإߧ���" << endl;

    // �إ߽ҵ{ID����
    createCourseIndex(dataFiles, "course_id_index");
    cout << "�ҵ{ID���ޫإߧ���" << endl;

    // �إ߽ҵ{�W�r����
    createCourseNameIndex(dataFiles, "course_name_index");
    cout << "�ҵ{�W�r���ޫإߧ���" << endl;


    return 0;
}

