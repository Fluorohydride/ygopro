--連鎖除外
function c57139487.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c57139487.target)
	e1:SetOperation(c57139487.activate)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e3)
end
function c57139487.filter(c)
	return c:IsFaceup() and c:GetAttack()<=1000 and c:IsAbleToRemove()
end
function c57139487.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chk==0 then return eg:IsExists(c57139487.filter,1,nil) end
	local g=eg:Filter(c57139487.filter,nil)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c57139487.efilter(c,e)
	return c:IsFaceup() and c:IsAttackBelow(1000) and c:IsRelateToEffect(e)
end
function c57139487.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(c57139487.efilter,nil,e)
	Duel.Remove(sg,POS_FACEUP,REASON_EFFECT)
	local rg=Group.CreateGroup()
	local tc=sg:GetFirst()
	while tc do
		if tc:IsLocation(LOCATION_REMOVED) then
			local tpe=tc:GetType()
			if bit.band(tpe,TYPE_TOKEN)==0 then
				local g1=Duel.GetMatchingGroup(Card.IsCode,tp,0,LOCATION_DECK+LOCATION_HAND,nil,tc:GetCode())
				rg:Merge(g1)
			end
		end
		tc=sg:GetNext()
	end
	if rg:GetCount()>0 then
		Duel.BreakEffect()
		Duel.Remove(rg,POS_FACEUP,REASON_EFFECT)
	end
end
