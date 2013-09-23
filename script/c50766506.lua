--忍法 分身の術
function c50766506.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetTarget(c50766506.target)
	e1:SetOperation(c50766506.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetOperation(c50766506.desop)
	c:RegisterEffect(e2)
end
function c50766506.cfilter(c,e,tp)
	local lv=c:GetLevel()
	return lv>0 and c:IsSetCard(0x2b)
		and Duel.IsExistingMatchingCard(c50766506.spfilter,tp,LOCATION_DECK,0,1,nil,lv,e,tp)
end
function c50766506.spfilter(c,lv,e,tp)
	return c:IsLevelBelow(lv) and c:IsSetCard(0x2b) and
		(c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP_ATTACK) or c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN_DEFENCE))
end
function c50766506.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1 and Duel.CheckReleaseGroup(tp,c50766506.cfilter,1,nil,e,tp) end
	local rg=Duel.SelectReleaseGroup(tp,c50766506.cfilter,1,1,nil,e,tp)
	e:SetLabel(rg:GetFirst():GetLevel())
	Duel.Release(rg,REASON_COST)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c50766506.spfilter2(c,e,tp)
	return c:IsSetCard(0x2b) and
		(c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP_ATTACK) or c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN_DEFENCE))
end
function c50766506.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	local c=e:GetHandler()
	local slv=e:GetLabel()
	local sg=Duel.GetMatchingGroup(c50766506.spfilter2,tp,LOCATION_DECK,0,nil,e,tp)
	sg:Remove(Card.IsLevelAbove,nil,slv+1)
	if sg:GetCount()==0 then return end
	local cg=Group.CreateGroup()
	repeat
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local tc=sg:Select(tp,1,1,nil):GetFirst()
		sg:RemoveCard(tc)
		slv=slv-tc:GetLevel()
		local spos=0
		if tc:IsCanBeSpecialSummoned(e,0,tp,false,false) then spos=spos+POS_FACEUP_ATTACK end
		if tc:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN) then spos=spos+POS_FACEDOWN_DEFENCE end
		Duel.SpecialSummonStep(tc,0,tp,tp,false,false,spos)
		if tc:IsFacedown() then cg:AddCard(tc) end
		c:SetCardTarget(tc)
		sg:Remove(Card.IsLevelAbove,nil,slv+1)
		ft=ft-1
	until ft<=0 or sg:GetCount()==0 or not Duel.SelectYesNo(tp,aux.Stringid(50766506,0))
	Duel.SpecialSummonComplete()
	Duel.ConfirmCards(1-tp,cg)
end
function c50766506.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetHandler():GetCardTarget():Filter(Card.IsLocation,nil,LOCATION_MZONE)
	Duel.Destroy(g,REASON_EFFECT)
end
