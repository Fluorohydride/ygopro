--チェイン・スラッシャー
function c88190453.initial_effect(c)
	--multiattack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetValue(c88190453.val)
	c:RegisterEffect(e1)
end
function c88190453.val(e,c)
	return Duel.GetMatchingGroupCount(Card.IsCode,e:GetHandlerPlayer(),LOCATION_GRAVE,0,nil,88190453)
end
