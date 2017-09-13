/**
 * mongoose������(��װmongodb)
 */

var fs = require('fs');
var path = require('path');
var mongoose = require('mongoose');
var logger = require('pomelo-logger').getLogger('mongodb-log');

var options = {
    db_user: "",
    db_pwd: "",
    db_host: "127.0.0.1",
    db_port: 27017,
    db_name: "dbname"
};

var dbURL = "mongodb://" + options.db_user + ":" + options.db_pwd + "@" + options.db_host + ":" + options.db_port + "/" + options.db_name;

mongoose.connect(dbURL);

mongoose.connection.on('connected', function (err) {
    if (err) logger.error('Database connection failure');
});

mongoose.connection.on('error', function (err) {
    logger.error('Mongoose connected error ' + err);
});

mongoose.connection.on('disconnected', function () {
    logger.error('Mongoose disconnected');
});

process.on('SIGINT', function () {
    mongoose.connection.close(function () {
        logger.info('Mongoose disconnected through app termination');
        process.exit(0);
    });
});

var DB = function () {
    this.mongoClient = {};
    //var filename = path.join(path.dirname(__dirname).replace('app', ''), './config/table.json');
    var filename = path.join(path.dirname(__dirname), 'mongodb/config/table.json');
    this.tabConf = JSON.parse(fs.readFileSync(path.normalize(filename)));
};

/**
 * ��ʼ��mongoose model
 * @param table_name ������(��������)
 */
DB.prototype.getConnection = function (table_name) {
    if (!table_name) return;
    if (!this.tabConf[table_name]) {
        logger.error('No table structure');
        return false;
    }

    var client = this.mongoClient[table_name];
    if (!client) {
        //�����û���Ϣ��ṹ
        var nodeSchema = new mongoose.Schema(this.tabConf[table_name]);

        //����model
        client = mongoose.model(table_name, nodeSchema, table_name);

        this.mongoClient[table_name] = client;
    }
    return client;
};

/**
 * ��������
 * @param table_name ����
 * @param fields ������
 * @param callback �ص�����
 */
DB.prototype.save = function (table_name, fields, callback) {
    if (!fields) {
        if (callback) callback({msg: 'Field is not allowed for null'});
        return false;
    }

    var err_num = 0;
    for (var i in fields) {
        if (!this.tabConf[table_name][i]) err_num ++;
    }
    if (err_num > 0) {
        if (callback) callback({msg: 'Wrong field name'});
        return false;
    }

    var node_model = this.getConnection(table_name);
    var mongooseEntity = new node_model(fields);
    mongooseEntity.save(function (err, res) {
        if (err) {
            if (callback) callback(err);
        } else {
            if (callback) callback(null, res);
        }
    });
};

/**
 * ��������
 * @param table_name ����
 * @param conditions ������Ҫ������ {_id: id, user_name: name}
 * @param update_fields Ҫ���µ��ֶ� {age: 21, sex: 1}
 * @param callback �ص�����
 */
DB.prototype.update = function (table_name, conditions, update_fields, callback) {
    if (!update_fields || !conditions) {
        if (callback) callback({msg: 'Parameter error'});
        return;
    }
    var node_model = this.getConnection(table_name);
    node_model.update(conditions, {$set: update_fields}, {multi: true, upsert: true}, function (err, res) {
        if (err) {
            if (callback) callback(err);
        } else {
            if (callback) callback(null, res);
        }
    });
};

/**
 * �������ݷ���(����������)
 * @param table_name ���ݱ���
 * @param conditions �������� {_id: id, user_name: name}
 * @param update_fields ���µĲ����� {$set: {id: 123}}
 * @param callback �ص�����
 */
DB.prototype.updateData = function (table_name, conditions, update_fields, callback) {
    if (!update_fields || !conditions) {
        if (callback) callback({msg: 'Parameter error'});
        return;
    }
    var node_model = this.getConnection(table_name);
    node_model.findOneAndUpdate(conditions, update_fields, {multi: true, upsert: true}, function (err, data) {
        if (callback) callback(err, data);
    });
};

/**
 * ɾ������
 * @param table_name ����
 * @param conditions ɾ����Ҫ������ {_id: id}
 * @param callback �ص�����
 */
DB.prototype.remove = function (table_name, conditions, callback) {
    var node_model = this.getConnection(table_name);
    node_model.remove(conditions, function (err, res) {
        if (err) {
            if (callback) callback(err);
        } else {
            if (callback) callback(null, res);
        }
    });
};

/**
 * ��ѯ����
 * @param table_name ����
 * @param conditions ��ѯ����
 * @param fields �������ֶ�
 * @param callback �ص�����
 */
DB.prototype.find = function (table_name, conditions, fields, callback) {
    var node_model = this.getConnection(table_name);
    node_model.find(conditions, fields || null, {}, function (err, res) {
        if (err) {
            callback(err);
        } else {
            callback(null, res);
        }
    });
};

/**
 * ��ѯ��������
 * @param table_name ����
 * @param conditions ��ѯ����
 * @param callback �ص�����
 */
DB.prototype.findOne = function (table_name, conditions, callback) {
    var node_model = this.getConnection(table_name);
    node_model.findOne(conditions, function (err, res) {
        if (err) {
            callback(err);
        } else {
            callback(null, res);
        }
    });
};

/**
 * ����_id��ѯָ��������
 * @param table_name ����
 * @param _id �������ַ����� ObjectId ����
 * @param callback �ص�����
 */
DB.prototype.findById = function (table_name, _id, callback) {
    var node_model = this.getConnection(table_name);
    node_model.findById(_id, function (err, res){
        if (err) {
            callback(err);
        } else {
            callback(null, res);
        }
    });
};

/**
 * ���ط����������ĵ���
 * @param table_name ����
 * @param conditions ��ѯ����
 * @param callback �ص�����
 */
DB.prototype.count = function (table_name, conditions, callback) {
    var node_model = this.getConnection(table_name);
    node_model.count(conditions, function (err, res) {
        if (err) {
            callback(err);
        } else {
            callback(null, res);
        }
    });
};

/**
 * ��ѯ�����������ĵ������ظ��ݼ�����Ľ��
 * @param table_name ����
 * @param field �����صļ�ֵ
 * @param conditions ��ѯ����
 * @param callback �ص�����
 */
DB.prototype.distinct = function (table_name, field, conditions, callback) {
    var node_model = this.getConnection(table_name);
    node_model.distinct(field, conditions, function (err, res) {
        if (err) {
            callback(err);
        } else {
            callback(null, res);
        }
    });
};

/**
 * ��д��ѯ
 * @param table_name ����
 * @param conditions ��ѯ���� {a:1, b:2}
 * @param options ѡ�{fields: "a b c", sort: {time: -1}, limit: 10}
 * @param callback �ص�����
 */
DB.prototype.where = function (table_name, conditions, options, callback) {
    var node_model = this.getConnection(table_name);
    node_model.find(conditions)
        .select(options.fields || '')
        .sort(options.sort || {})
        .limit(options.limit || {})
        .exec(function (err, res) {
            if (err) {
                callback(err);
            } else {
                callback(null, res);
            }
        });
};

module.exports = new DB();
