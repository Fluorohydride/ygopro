--転身テンシーン
function c59563768.initial_effect(c)
	--atk
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetValue(c59563768.atkval)
	c:RegisterEffect(e2)
end
function c59563768.filter(c)
	return c:IsFaceup() and c:GetLevel()==2
end
function c59563768.atkval(e,c)
	return Duel.GetMatchingGroupCount(c59563768.filter,e:GetHandlerPlayer(),LOCATION_MZONE,0,nil)*400
end
