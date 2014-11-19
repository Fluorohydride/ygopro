--砲弾ヤリ貝
function c95614612.initial_effect(c)
	--immune spell
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_IMMUNE_EFFECT)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c95614612.econ)
	e1:SetValue(c95614612.efilter)
	c:RegisterEffect(e1)
end
function c95614612.filter(c)
	return c:IsFaceup() and c:IsCode(22702055)
end
function c95614612.econ(e)
	return Duel.IsExistingMatchingCard(c95614612.filter,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
		or Duel.IsEnvironment(22702055)
end
function c95614612.efilter(e,te)
	return te:IsActiveType(TYPE_SPELL)
end
