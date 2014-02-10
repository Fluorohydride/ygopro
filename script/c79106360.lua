--カオスポッド
function c79106360.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetOperation(c79106360.operation)
	c:RegisterEffect(e1)
end
function c79106360.filter(c)
	return not c:IsStatus(STATUS_BATTLE_DESTROYED) and c:IsAbleToDeck()
end
function c79106360.operation(e,tp,eg,ep,ev,re,r,rp)
	local rg=Duel.GetMatchingGroup(c79106360.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SendtoDeck(rg,nil,0,REASON_EFFECT)
	local ct1=0
	local ct2=0
	rg=Duel.GetOperatedGroup()
	local tc=rg:GetFirst()
	while tc do
		if tc:IsLocation(LOCATION_DECK) and tc:IsType(TYPE_MONSTER) then
			if tc:GetControler()==tp then ct1=ct1+1
			else ct2=ct2+1 end
		end
		tc=rg:GetNext()
	end
	if ct1>0 then Duel.ShuffleDeck(tp) end
	if ct2>0 then Duel.ShuffleDeck(1-tp) end
	Duel.BreakEffect()
	local g1=nil
	local g2=nil
	if ct1>0 then g1=c79106360.sp(e,tp,ct1) end
	if ct2>0 then g2=c79106360.sp(e,1-tp,ct2) end
	Duel.SpecialSummonComplete()
	if g1 then Duel.ShuffleSetCard(g1) end
	if g2 then Duel.ShuffleSetCard(g2) end
end
function c79106360.spfilter(c,e,tp)
	local lv=c:GetLevel()
	return lv>0 and lv<=4 and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN)
end
function c79106360.sp(e,tp,ct)
	local g=Duel.GetFieldGroup(tp,LOCATION_DECK,0)
	local dt=g:GetCount()
	if dt==0 then return false end
	local dlist={}
	local tc=g:GetFirst()
	while tc do
		if tc:IsType(TYPE_MONSTER) then dlist[tc:GetSequence()]=tc end
		tc=g:GetNext()
	end
	local i=dt-1
	local a=0
	local last=nil
	g=Group.CreateGroup()
	while a<ct and i>=0 do
		tc=dlist[i]
		if tc then
			g:AddCard(tc)
			last=tc
			a=a+1
		end
		i=i-1
	end
	local conf=dt-last:GetSequence()
	Duel.ConfirmDecktop(tp,conf)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	g=g:Filter(c79106360.spfilter,nil,e,tp)
	if g:GetCount()>ft then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		g=g:Select(tp,ft,ft,nil)
	end
	tc=g:GetFirst()
	while tc do
		Duel.DisableShuffleCheck()
		Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
		tc=g:GetNext()
	end
	if conf-g:GetCount()>0 then
		Duel.DiscardDeck(tp,conf-g:GetCount(),REASON_EFFECT+REASON_REVEAL)
	end
	return g
end
