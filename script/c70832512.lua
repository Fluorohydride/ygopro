--レアル・クルセイダー
function c70832512.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(70832512,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCost(c70832512.cost)
	e1:SetTarget(c70832512.target)
	e1:SetOperation(c70832512.operation)
	c:RegisterEffect(e1)
end
function c70832512.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) and e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c70832512.dfilter(c,e)
	return c:IsFaceup() and c:IsLevelAbove(5) and (not e or c:IsRelateToEffect(e))
end
function c70832512.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if not eg:IsExists(c70832512.dfilter,1,nil) then return false end
		if e:GetHandler():IsStatus(STATUS_CHAINING) then return false end
		e:GetHandler():SetStatus(STATUS_CHAINING,true)
		return true
	end
	Duel.SetTargetCard(eg)
end
function c70832512.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c70832512.dfilter,nil,e)
	if g:GetCount()~=0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
