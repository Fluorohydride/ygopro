--ダイスロール・バトル
function c88482761.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c88482761.condition)
	e1:SetTarget(c88482761.target)
	e1:SetOperation(c88482761.operation)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(88482761,0))
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetCondition(c88482761.atkcon)
	e2:SetCost(c88482761.atkcost)
	e2:SetTarget(c88482761.atktg)
	e2:SetOperation(c88482761.atkop)
	c:RegisterEffect(e2)
end
function c88482761.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c88482761.rmfilter1(c,e,tp)
	return c:IsSetCard(0x2016) and c:IsAbleToRemove()
		and Duel.IsExistingMatchingCard(c88482761.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp,c:GetOriginalLevel())
end
function c88482761.rmfilter2(c,lv)
	return c:IsSetCard(0x2016) and c:IsType(TYPE_TUNER) and c:IsAbleToRemove() and c:GetOriginalLevel()==lv
end
function c88482761.spfilter(c,e,tp,lv)
	return c:IsType(TYPE_SYNCHRO) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and Duel.IsExistingMatchingCard(c88482761.rmfilter2,tp,LOCATION_HAND,0,1,nil,c:GetLevel()-lv)
end
function c88482761.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c88482761.rmfilter1(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c88482761.rmfilter1,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c88482761.rmfilter1,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c88482761.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c88482761.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,tc:GetOriginalLevel())
	if g:GetCount()>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local rg=Duel.SelectMatchingCard(tp,c88482761.rmfilter2,tp,LOCATION_HAND,0,1,1,nil,g:GetFirst():GetLevel()-tc:GetOriginalLevel())
		rg:AddCard(tc)
		if Duel.Remove(rg,POS_FACEUP,REASON_EFFECT)==2 then
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
function c88482761.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetCurrentPhase()==PHASE_BATTLE
		and not Duel.CheckTiming(TIMING_BATTLE_START+TIMING_BATTLE_END)
end
function c88482761.atkcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c88482761.atkfilter(c)
	return c:IsType(TYPE_SYNCHRO) and c:IsPosition(POS_FACEUP_ATTACK)
end
function c88482761.atktg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c88482761.atkfilter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(c88482761.atkfilter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c88482761.atkfilter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g2=Duel.SelectTarget(tp,c88482761.atkfilter,tp,0,LOCATION_MZONE,1,1,nil)
	e:SetLabelObject(g2:GetFirst())
end
function c88482761.atkop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if g:GetCount()==2 then
		local c1=g:GetFirst()
		local c2=g:GetNext()
		if c1~=e:GetLabelObject() then c1,c2=c2,c1 end
		if c1:IsControler(1-tp) and c1:IsPosition(POS_FACEUP_ATTACK) and not c1:IsImmuneToEffect(e)
			and c2:IsControler(tp) then
			Duel.CalculateDamage(c1,c2)
		end
	end
end
