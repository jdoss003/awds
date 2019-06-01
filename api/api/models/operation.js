const mongoose = require('mongoose');

const Operation = mongoose.Schema({
    DATE_ENTERED: {
        type: Date,
        default: Date.now,
    },
    OWNER: {
        type: String,
        required: true,
    },
    OPERATIONS: [{ type: mongoose.Schema.Types.ObjectId, ref: 'Package' }],
});

module.exports = mongoose.model('Operation', Operation);