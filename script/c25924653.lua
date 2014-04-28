--剣闘獣ダリウス
function c25924653.initial_effect(c)
	--grave special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(25924653,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c25924653.spgcon)
	e1:SetTarget(c25924653.spgtg)
	e1:SetOperation(c25924653.spgop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(25924653,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c25924653.spcon)
	e2:SetCost(c25924653.spcost)
	e2:SetTarget(c25924653.sptg)
	e2:SetOperation(c25924653.spop)
	c:RegisterEffect(e2)
	--leave field
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetOperation(c25924653.leave)
	c:RegisterEffect(e3)
end
function c25924653.spgcon(e,tp,eg,ep,ev,re,r,rp)
	local st=e:GetHandler():GetSummonType()
	return st>=(SUMMON_TYPE_SPECIAL+100) and st<(SUMMON_TYPE_SPECIAL+150)
end
function c25924653.spgfilter(c,e,tp)
	return c:IsSetCard(0x19) and c:IsCanBeSpecialSummoned(e,104,tp,false,false)
end
function c25924653.spgtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c25924653.spgfilter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c25924653.spgfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c25924653.spgfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c25924653.spgop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and Duel.SpecialSummon(tc,104,tp,tp,false,false,POS_FACEUP)>0
		and c:IsFaceup() and c:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		tc:RegisterFlagEffect(25924654,RESET_EVENT+0x53e0000,0,1)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_OWNER_RELATE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCondition(c25924653.discon)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_CHAIN_SOLVING)
		e2:SetRange(LOCATION_MZONE)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetOperation(c25924653.disop)
		c:RegisterEffect(e2)
	end
end
function c25924653.discon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
function c25924653.disop(e,tp,eg,ep,ev,re,r,rp)
	if re:GetHandler():GetFlagEffect(25924654)~=0 then
		Duel.NegateEffect(ev)
	end
end
function c25924653.leave(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=c:GetFirstCardTarget()
	if tc and tc:IsLocation(LOCATION_MZONE) and tc:GetFlagEffect(25924654)~=0 then
		Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)
	end
end
function c25924653.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetBattledGroupCount()>0
end
function c25924653.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsAbleToDeckAsCost() end
	Duel.SendtoDeck(c,nil,2,REASON_COST)
end
function c25924653.filter(c,e,tp)
	return not c:IsCode(25924653) and c:IsSetCard(0x19) and c:IsCanBeSpecialSummoned(e,104,tp,false,false)
end
function c25924653.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c25924653.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c25924653.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c25924653.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	local tc=g:GetFirst()
	if tc then
		Duel.SpecialSummon(tc,104,tp,tp,false,false,POS_FACEUP)		
		tc:RegisterFlagEffect(tc:GetOriginalCode(),RESET_EVENT+0x1ff0000,0,0)
	end
end
