--王虎ワンフー
function c83986578.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(83986578,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c83986578.condition)
	e1:SetTarget(c83986578.target)
	e1:SetOperation(c83986578.operation)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
end
function c83986578.cfilter(c)
	return c:IsFaceup() and c:IsAttackBelow(1400)
end
function c83986578.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c83986578.cfilter,1,nil) and not eg:IsContains(e:GetHandler())
end
function c83986578.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=eg:Filter(c83986578.cfilter,nil)
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c83986578.filter(c,e)
	return c:IsFaceup() and c:IsAttackBelow(1400) and c:IsRelateToEffect(e)
end
function c83986578.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=eg:Filter(c83986578.filter,nil,e)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
