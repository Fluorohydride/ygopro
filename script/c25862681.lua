--エンシェント・フェアリー・ドラゴン
function c25862681.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(25862681,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c25862681.sumcon)
	e1:SetCost(c25862681.cost)
	e1:SetTarget(c25862681.sumtg)
	e1:SetOperation(c25862681.sumop)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(25862681,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCategory(CATEGORY_DESTROY+CATEGORY_RECOVER+CATEGORY_SEARCH)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c25862681.cost)
	e2:SetTarget(c25862681.destg)
	e2:SetOperation(c25862681.desop)
	c:RegisterEffect(e2)
end
function c25862681.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
function c25862681.sumcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c25862681.sumfilter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c25862681.sumtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c25862681.sumfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,LOCATION_HAND)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_BP)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetTargetRange(1,0)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c25862681.sumop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c25862681.sumfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c25862681.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c1=Duel.GetFieldCard(0,LOCATION_SZONE,5)
	local c2=Duel.GetFieldCard(1,LOCATION_SZONE,5)
	if chk==0 then return (c1 and c1:IsDestructable()) or (c2 and c2:IsDestructable()) end
	local g=Group.CreateGroup()
	if c1 and c1:IsDestructable() then g:AddCard(c1) end
	if c2 and c2:IsDestructable() then g:AddCard(c2) end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,1000)
end
function c25862681.ffilter(c)
	return c:IsType(TYPE_FIELD) and c:IsAbleToHand()
end
function c25862681.desop(e,tp,eg,ep,ev,re,r,rp)
	local c1=Duel.GetFieldCard(0,LOCATION_SZONE,5)
	local c2=Duel.GetFieldCard(1,LOCATION_SZONE,5)
	local g=Group.CreateGroup()
	if c1 and c1:IsDestructable() then g:AddCard(c1) end
	if c2 and c2:IsDestructable() then g:AddCard(c2) end
	if g:GetCount()>0 then
		local ct=Duel.Destroy(g,REASON_EFFECT)
		if ct>0 then
			Duel.Recover(tp,1000,REASON_EFFECT)
			local fg=Duel.GetMatchingGroup(c25862681.ffilter,tp,LOCATION_DECK,0,nil)
			if fg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(25862681,2)) then
				Duel.BreakEffect()
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
				local sg=fg:Select(tp,1,1,nil)
				Duel.SendtoHand(sg,nil,REASON_EFFECT)
				Duel.ConfirmCards(1-tp,sg)
			end
		end
	end
end
