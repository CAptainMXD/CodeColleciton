var MongoDB = require('./mongodb');

MongoDB.findOne('user_info', {_id: user_id}, function (err, res) {
    console.log(res);
});

MongoDB.find('user_info', {type: 1}, {}, function (err, res) {
    console.log(res);
});

MongoDB.updateData('user_info', {_id: user_info._id}, {$set: update_data}, function(err, user_info) {
      callback(null, user_info);
});

MongoDB.remove('user_data', {user_id: 1});
