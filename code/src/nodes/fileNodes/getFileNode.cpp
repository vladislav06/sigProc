//
// Created by vm on 24.2.12.
//

#include "getFileNode.h"

QString GetFileNode::caption() const {
    return "Get files";
}

QString GetFileNode::name() const {
    return "Get files";
}

QWidget *GetFileNode::embeddedWidget() {
    return nullptr;
}

std::tuple<std::shared_ptr<ArrayDataType<std::shared_ptr<FileDataType>>>>
GetFileNode::compute(std::tuple<std::shared_ptr<FolderDataType>> params,
                     std::vector<std::shared_ptr<BaseDataType>> adParams) {
    auto folder = std::get<0>(params);
    if (folder == nullptr) {
        return {};
    }

    return folder->getFiles();
}

QJsonObject GetFileNode::onSave() const {
    return {};
}

bool GetFileNode::onLoad(QJsonObject json) {
    return false;
}
