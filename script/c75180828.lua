--水精鱗－サルフアビス
function c75180828.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(75180828,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCost(c75180828.spcost)
	e1:SetTarget(c75180828.sptg)
	e1:SetOperation(c75180828.spop)
	c:RegisterEffect(e1)
	--atkup & destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(75180828,1))
	e2:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c75180828.descon)
	e2:SetTarget(c75180828.destg)
	e2:SetOperation(c75180828.desop)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(75180828,2))
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCondition(c75180828.atkcon)
	e3:SetCost(c75180828.atkcost)
	e3:SetTarget(c75180828.atktg)
	e3:SetOperation(c75180828.atkop)
	c:RegisterEffect(e3)
end
function c75180828.cfilter(c)
	return c:IsSetCard(0x74) and c:IsDiscardable() and c:IsAbleToGraveAsCost()
end
function c75180828.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c75180828.cfilter,tp,LOCATION_HAND,0,4,e:GetHandler()) end
	Duel.DiscardHand(tp,c75180828.cfilter,4,4,REASON_COST+REASON_DISCARD,e:GetHandler())
end
function c75180828.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c75180828.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,1,tp,tp,false,false,POS_FACEUP)
	end
end
function c75180828.descon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SPECIAL+1
end
function c75180828.descount(c)
	return c:IsSetCard(0x74) and c:IsType(TYPE_MONSTER)
end
function c75180828.desfilter(c)
	return c:IsDestructable()
end
function c75180828.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsOnField() and c75180828.desfilter(chkc) end
	if chk==0 then return true end
	local ct=Duel.GetMatchingGroupCount(c75180828.descount,tp,LOCATION_GRAVE,0,nil)
	if ct>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectTarget(tp,c75180828.desfilter,tp,0,LOCATION_ONFIELD,1,ct,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	end
end
function c75180828.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(500)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.Destroy(g,REASON_EFFECT)
end
function c75180828.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c75180828.rfilter(c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsSetCard(0x74)
end
function c75180828.atkcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c75180828.rfilter,1,e:GetHandler()) end
	local g=Duel.SelectReleaseGroup(tp,c75180828.rfilter,1,1,e:GetHandler())
	Duel.Release(g,REASON_COST)
end
function c75180828.atktg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(75180828)==0 end
end
function c75180828.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		--destroy
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(75180828,3))
		e1:SetCategory(CATEGORY_DESTROY)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_BATTLE_START)
		e1:SetTarget(c75180828.destg2)
		e1:SetOperation(c75180828.desop2)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,1)
		c:RegisterEffect(e1)
		c:RegisterFlagEffect(75180828,RESET_PHASE+PHASE_END,0,1)
	end
end
function c75180828.destg2(e,tp,eg,ep,ev,re,r,rp,chk)
	local d=Duel.GetAttackTarget()
	if chk ==0 then	return Duel.GetAttacker()==e:GetHandler() and d~=nil and d:IsDefencePos() end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,d,1,0,0)
end
function c75180828.desop2(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	if d~=nil and d:IsRelateToBattle() and d:IsDefencePos() then
		Duel.Destroy(d,REASON_EFFECT)
	end
end
