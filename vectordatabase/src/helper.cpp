/*
 *Copyright (c) 2024, Tencent. All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of elasticfaiss nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "include/helper.h"
#include "include/rpc_client.h"
#include "include/types/collection.h"
#include "include/types/document.h"

namespace vectordb {

void toCollection(const olama::CreateCollectionRequest& collectionItem, Collection* collection) {
    collection->database = collectionItem.database();
    collection->collectionName = collectionItem.collection();
    collection->documentCount = collectionItem.size();
    collection->alias.assign(collectionItem.alias_list().begin(), collectionItem.alias_list().end());
    collection->shardNum = collectionItem.shardnum();
    collection->replicaNum = collectionItem.replicanum();
    collection->description = collectionItem.description();
    collection->size = collectionItem.size();

    if (collectionItem.has_embeddingparams()) {
        collection->embedding.field = collectionItem.embeddingparams().field();
        collection->embedding.vectorField = collectionItem.embeddingparams().vector_field();
        collection->embedding.model = collectionItem.embeddingparams().model_name();
        collection->embedding.enabled = false;
    }
    if (collectionItem.has_indexstatus()) {
        collection->indexStatus.status = collectionItem.indexstatus().status();
        collection->indexStatus.startTime = collectionItem.indexstatus().starttime();
    }
    collection->createTime = collectionItem.createtime();
    for (const auto& [key, index] : collectionItem.indexes()) {
        if (index.fieldtype() == kVector) {
            VectorIndex vector;
            vector.fieldName = index.fieldname();
            vector.fieldType = index.fieldtype();
            vector.indexType = index.indextype();
            vector.dimension = index.dimension();
            vector.metricType = index.metrictype();
            vector.indexCount = collectionItem.size();
            if (index.has_params()) {
                vector.params.m = index.params().m();
                vector.params.efConstruction = index.params().efconstruction();
                vector.params.nList = index.params().nlist();
                vector.params.nProbe = index.params().nprobe();
            }
            collection->indexes.vectorIndex.emplace_back(vector);
        } else if (index.fieldtype() == kArray) {
            FilterIndex filter;
            filter.fieldName = index.fieldname();
            filter.fieldType = index.fieldtype();
            filter.indexType = index.indextype();
            filter.elemType = index.fieldelementtype();
            collection->indexes.filterIndex.emplace_back(filter);
        } else {
            FilterIndex filter;
            filter.fieldName = index.fieldname();
            filter.fieldType = index.fieldtype();
            filter.indexType = index.indextype();
            collection->indexes.filterIndex.emplace_back(filter);
        }
    }
}

void convertField2Proto(const Field& field, olama::Field* protoField) {
    if (field.hasValDouble()) {
        protoField->set_val_double(field.getValDouble());
    } else if (field.hasValStr()) {
        protoField->set_val_str(field.getValStr());
    } else if (field.hasValU64()) {
        protoField->set_val_u64(field.getValU64());
    } else if (field.hasValStrArr()) {
        for (const auto& str : field.getValStrArr()) {
            protoField->mutable_val_str_arr()->add_str_arr(str);
        }
    }
}

void convertProto2Field(const olama::Field& protoField, Field* field) {
    if (!protoField.has_val_double() && !protoField.has_val_str() &&
        !protoField.has_val_str_arr() && protoField.has_val_u64()) return;
    switch (protoField.oneof_val_case()) {
        case olama::Field::OneofValCase::kValStr:
            field->setValStr(protoField.val_str());
            break;
        case olama::Field::OneofValCase::kValU64:
            field->setValU64(protoField.val_u64());
            break;
        case olama::Field::OneofValCase::kValDouble:
            field->setValDouble(protoField.val_double());
            break;
        case olama::Field::OneofValCase::kValStrArr: {
            std::vector<std::string> strArr;
            for (const auto& str : protoField.val_str_arr().str_arr()) {
                strArr.push_back(str);
            }
            field->setValStrArr(strArr);
            break;
        }
        default:
            break;
    }
}

}  // namespace vectordb
