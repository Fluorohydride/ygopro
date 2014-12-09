--姑息な落とし穴
function c51976476.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetTarget(c51976476.target)
	e1:SetOperation(c51976476.activate)
	c:RegisterEffect(e1)
end
function c51976476.filter(c,e,tp)
	return c:GetSummonPlayer()==tp and c:IsDefencePos() and c:IsAbleToRemove()
		and (not e or (c:IsRelateToEffect(e) and c:IsLocation(LOCATION_MZONE)))
end
function c51976476.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c51976476.filter,1,nil,nil,1-tp) end
	local g=eg:Filter(c51976476.filter,nil,nil,1-tp)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c51976476.activate(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local g=tg:Filter(c51976476.filter,nil,e,1-tp)
	if g:GetCount()>0 then
		Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
	end
end
