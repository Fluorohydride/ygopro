--カトブレパスと運命の魔女
function c54360049.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(54360049,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c54360049.cost)
	e1:SetTarget(c54360049.target)
	e1:SetOperation(c54360049.operation)
	c:RegisterEffect(e1)
end
function c54360049.cfilter(c)
	return c:GetAttack()==-2 and c:IsAbleToRemoveAsCost()
end
function c54360049.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c54360049.cfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c54360049.cfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c54360049.filter(c,tp)
	return c:GetSummonPlayer()~=tp and c:IsDestructable()
end
function c54360049.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c54360049.filter,1,nil,tp) end
	local g=eg:Filter(c54360049.filter,nil,tp)
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c54360049.dfilter(c,e,tp)
	return c:GetSummonPlayer()~=tp and c:IsDestructable() and c:IsRelateToEffect(e)
end
function c54360049.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c54360049.dfilter,nil,e,tp)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
