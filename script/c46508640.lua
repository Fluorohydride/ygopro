--龍脈に棲む者
function c46508640.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c46508640.atkval)
	c:RegisterEffect(e1)
end
function c46508640.cfilter(c)
	return c:IsFaceup() and c:GetType()==TYPE_SPELL+TYPE_CONTINUOUS and c:GetSequence()~=5
end
function c46508640.atkval(e,c)
	return Duel.GetMatchingGroupCount(c46508640.cfilter,c:GetControler(),LOCATION_SZONE,0,nil)*300
end
