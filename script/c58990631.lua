--オートマチック・レーザー
function c58990631.initial_effect(c)
	--Activate(summon)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCost(c58990631.cost)
	e1:SetTarget(c58990631.target)
	e1:SetOperation(c58990631.activate)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_ACTIVATE)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCost(c58990631.cost)
	e2:SetTarget(c58990631.target2)
	e2:SetOperation(c58990631.activate2)
	c:RegisterEffect(e2)
end
function c58990631.cfilter(c)
	return c:IsCode(36623431) and not c:IsPublic()
end
function c58990631.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c58990631.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local g=Duel.SelectMatchingCard(tp,c58990631.cfilter,tp,LOCATION_HAND,0,1,1,nil)
	Duel.ConfirmCards(1-tp,g)
	Duel.ShuffleHand(tp)
end
function c58990631.filter(c,tp,ep)
	return c:IsFaceup() and c:IsAttackAbove(1000)
		and ep~=tp and c:IsDestructable()
end
function c58990631.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=eg:GetFirst()
	if chk==0 then return c58990631.filter(tc,tp,ep) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,tc,1,0,0)
end
function c58990631.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc and tc:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsAttackAbove(1000) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c58990631.filter2(c,tp)
	return c:IsFaceup() and c:IsAttackAbove(1000) and c:GetSummonPlayer()~=tp
		and c:IsDestructable()
end
function c58990631.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c58990631.filter2,1,nil,tp) end
	local g=eg:Filter(c58990631.filter2,nil,tp)
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c58990631.filter3(c,e,tp)
	return c:IsFaceup() and c:IsAttackAbove(1000) and c:GetSummonPlayer()~=tp
		and c:IsRelateToEffect(e) and c:IsDestructable()
end
function c58990631.activate2(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c58990631.filter3,nil,e,tp)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
