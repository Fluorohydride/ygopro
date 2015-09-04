--ヴォルカニック・カウンター
function c66436257.initial_effect(c)
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(66436257,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c66436257.condition)
	e1:SetTarget(c66436257.target)
	e1:SetOperation(c66436257.operation)
	c:RegisterEffect(e1)
end
function c66436257.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp
end
function c66436257.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,e:GetHandler(),1,0,0)
end
function c66436257.filter(c)
	return c:IsAttribute(ATTRIBUTE_FIRE) and c:GetCode()~=66436257
end
function c66436257.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_EFFECT)~=0
		and Duel.IsExistingMatchingCard(c66436257.filter,tp,LOCATION_GRAVE,0,1,nil) then
		Duel.Damage(1-tp,ev,REASON_EFFECT)
	end
end
