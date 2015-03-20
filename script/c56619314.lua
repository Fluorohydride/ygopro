--DDD覇龍王ペンドラゴン
function c56619314.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(56619314,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCost(c56619314.spcost)
	e1:SetTarget(c56619314.sptg)
	e1:SetOperation(c56619314.spop)
	c:RegisterEffect(e1)
	--atk up/destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(56619314,1))
	e2:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c56619314.cost)
	e2:SetOperation(c56619314.operation)
	c:RegisterEffect(e2)
end
function c56619314.spfilter(c,rac,tp)
	return c:IsRace(rac) and c:IsControler(tp)
end
function c56619314.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local ct=0
		if Duel.CheckReleaseGroup(tp,c56619314.spfilter,1,nil,RACE_DRAGON,tp) then ct=ct-1 end
		if Duel.CheckReleaseGroup(tp,c56619314.spfilter,1,nil,RACE_FIEND,tp) then ct=ct-1 end
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>ct
			and Duel.CheckReleaseGroupEx(tp,Card.IsRace,1,e:GetHandler(),RACE_DRAGON)
			and Duel.CheckReleaseGroupEx(tp,Card.IsRace,1,e:GetHandler(),RACE_FIEND)
	end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft>0 then
		local g1=Duel.SelectReleaseGroupEx(tp,Card.IsRace,1,1,e:GetHandler(),RACE_DRAGON)
		local g2=Duel.SelectReleaseGroupEx(tp,Card.IsRace,1,1,e:GetHandler(),RACE_FIEND)
		g1:Merge(g2)
		Duel.Release(g1,REASON_COST)
	elseif ft==0 then
		local g1=Duel.SelectReleaseGroup(tp,c56619314.spfilter,1,1,nil,RACE_DRAGON+RACE_FIEND,tp)
		local rac=RACE_DRAGON
		if g1:GetFirst():IsRace(RACE_DRAGON) then rac=RACE_FIEND end
		local g2=Duel.SelectReleaseGroupEx(tp,Card.IsRace,1,1,e:GetHandler(),rac)
		g1:Merge(g2)
		Duel.Release(g1,REASON_COST)
	else
		local g1=Duel.SelectReleaseGroup(tp,c56619314.spfilter,1,1,nil,RACE_DRAGON,tp)
		local g2=Duel.SelectReleaseGroup(tp,c56619314.spfilter,1,1,nil,RACE_FIEND,tp)
		g1:Merge(g2)
		Duel.Release(g1,REASON_COST)
	end
end
function c56619314.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c56619314.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c56619314.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c56619314.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c56619314.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) and not c:IsImmuneToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(500)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
		local g=Duel.GetMatchingGroup(c56619314.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
		if g:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(56619314,2)) then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
			local dg=g:Select(tp,1,1,nil)
			Duel.HintSelection(dg)
			Duel.Destroy(dg,REASON_EFFECT)
		end
	end
end
