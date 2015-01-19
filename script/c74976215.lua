--クロスソード・ハンター
function c74976215.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCondition(c74976215.condition)
	e1:SetTarget(c74976215.target)
	c:RegisterEffect(e1)
end
function c74976215.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_INSECT)
end
function c74976215.condition(e)
	return Duel.IsExistingMatchingCard(c74976215.filter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c74976215.target(e,c)
	return c:IsRace(RACE_INSECT)
end
