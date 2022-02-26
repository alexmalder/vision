box.cfg {
    listen = "0.0.0.0:3301"
}
--test str
box.once("bootstrap", function()
    box.schema.space.create('account')
    box.space.account:format({
            {name='username', type='string'},
            {name='email', type='string'},
            {name='password', type='string'},
            {name='role', type='number'},
            {name='id', type='number'}
        })
    box.space.account:create_index('primary', {
            unique = true,
            parts = {
                {field=1, type='string'},
            }
        })

    box.schema.space.create('crypto')
    box.space.crypto:format({
            {name='unix', type='number'},
            {name='datetime', type='string'},
            {name='symbol', type='number'},
            {name='open', type='number'},
            {name='high', type='number'},
            {name='low', type='number'},
            {name='close', type='number'},
            {name='volume_original', type='number'},
            {name='volume_usd', type='number'}
        })
    box.space.crypto:create_index('primary', {
            unique = true,
            parts = {
                {field=3, type='number'},
                {field=1, type='number'},
            }
        })

    box.schema.space.create('contribution')
    box.space.contribution:format({
            {name='user_id', type='number'},
            {name='request_id', type='number'},
            {name='start_date', type='number'},
            {name='end_date', type='number'},
            {name='resolution', type='number'},
            {name='threshold', type='number'},
        })
    box.space.contribution:create_index('primary', {
            unique = true,
            parts = {
                {field=1, type='number'},
                {field=2, type='number'},
            }
        })

    box.schema.space.create('result')
    box.space.result:format({
            {name='user_id', type='number'},
            {name='request_id', type='number'},
            {name='unix', type='number'},
            {name='symbol', type='number'},
            {name='value', type='number'},
        })
    box.space.result:create_index('primary', {
            unique = true,
            parts = {
                {field=1, type='number'},
                {field=2, type='number'},
            }
        })

    box.schema.user.create('tnt_user', {password = 'dG50X3Bhc3N3b3JkCg'})
    box.schema.user.grant('tnt_user','read,write,execute,create,drop','universe')
end)
