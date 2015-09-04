--ネオフレムベル・サーベル
function c91554542.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c91554542.val)
	c:RegisterEffect(e1)
end
function c91554542.val(e)
	local gct=Duel.GetFieldGroupCount(e:GetHandler():GetControler(),0,LOCATION_GRAVE)
	if gct<=4 then return 600
	elseif gct>=8 then return -300
	else return 0 end
end
