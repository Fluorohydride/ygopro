--ブラック・ガーデン
function c71645242.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--token
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(71645242,0))
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetRange(LOCATION_FZONE)
	e4:SetCode(71645242)
	e4:SetTarget(c71645242.sptg)
	e4:SetOperation(c71645242.spop)
	c:RegisterEffect(e4)
	--special summon
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(71645242,1))
	e5:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e5:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e5:SetType(EFFECT_TYPE_IGNITION)
	e5:SetRange(LOCATION_FZONE)
	e5:SetTarget(c71645242.sptg2)
	e5:SetOperation(c71645242.spop2)
	c:RegisterEffect(e5)
	if not c71645242.global_check then
		c71645242.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SUMMON_SUCCESS)
		ge1:SetCondition(c71645242.regcon)
		ge1:SetOperation(c71645242.regop)
		Duel.RegisterEffect(ge1,0)
		local ge2=ge1:Clone()
		ge2:SetCode(EVENT_SPSUMMON_SUCCESS)
		Duel.RegisterEffect(ge2,0)
	end
end
function c71645242.cfilter(c,tp)
	return c:IsControler(tp) and c:GetSummonType()~=SUMMON_TYPE_SPECIAL+0x20
end
function c71645242.regcon(e,tp,eg,ep,ev,re,r,rp)
	local sf=0
	if eg:IsExists(c71645242.cfilter,1,nil,0) then
		sf=sf+1
	end
	if eg:IsExists(c71645242.cfilter,1,nil,1) then
		sf=sf+2
	end
	e:SetLabel(sf)
	return sf~=0
end
function c71645242.regop(e,tp,eg,ep,ev,re,r,rp)
	Duel.RaiseEvent(eg,71645242,e,r,rp,ep,e:GetLabel())
end
function c71645242.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c71645242.spop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=eg:Filter(Card.IsRelateToEffect,nil,e)
	local tc=g:GetFirst()
	while tc do
		if tc:IsFaceup() then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_SET_ATTACK_FINAL)
			e1:SetValue(math.ceil(tc:GetAttack()/2))
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
		end
		tc=g:GetNext()
	end
	if bit.band(bit.rshift(ev,tp),1)~=0 and Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,71645243,0,0x4011,800,800,2,RACE_PLANT,ATTRIBUTE_DARK,POS_FACEUP_ATTACK,1-tp) then
		local token=Duel.CreateToken(tp,71645243)
		Duel.SpecialSummonStep(token,0x20,tp,1-tp,false,false,POS_FACEUP_ATTACK)
	end
	if bit.band(bit.rshift(ev,1-tp),1)~=0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,71645243,0,0x4011,800,800,2,RACE_PLANT,ATTRIBUTE_DARK) then
		local token=Duel.CreateToken(1-tp,71645243)
		Duel.SpecialSummonStep(token,0x20,tp,tp,false,false,POS_FACEUP_ATTACK)
	end
	Duel.SpecialSummonComplete()
end
function c71645242.desfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_PLANT) and c:IsDestructable()
end
function c71645242.filter2(c,atk,e,tp)
	return c:GetAttack()==atk and c:IsCanBeSpecialSummoned(e,0x20,tp,false,false)
end
function c71645242.sptg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c71645242.filter2(chkc,e:GetLabel(),e,tp) end
	local g=Duel.GetMatchingGroup(c71645242.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local atk=g:GetSum(Card.GetAttack)
	local sc=g:FilterCount(Card.IsControler,nil,tp)
	if chk==0 then return e:GetHandler():IsDestructable() and e:GetHandler():IsDestructable(e) and g:GetCount()>0
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>-sc
		and Duel.IsExistingTarget(c71645242.filter2,tp,LOCATION_GRAVE,0,1,nil,atk,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local tg=Duel.SelectTarget(tp,c71645242.filter2,tp,LOCATION_GRAVE,0,1,1,nil,atk,e,tp)
	e:SetLabel(atk)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,tg,1,0,0)
	g:AddCard(e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c71645242.spop2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not (c:IsRelateToEffect(e) and c:IsDestructable() and c:IsDestructable(e)) then return end
	local dg=Duel.GetMatchingGroup(c71645242.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	dg:AddCard(c)
	Duel.Destroy(dg,REASON_EFFECT)
	Duel.BreakEffect()
	local og=Duel.GetOperatedGroup()
	og:RemoveCard(c)
	local atk=og:GetSum(Card.GetPreviousAttackOnField)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:GetAttack()==atk then
		Duel.SpecialSummon(tc,0x20,tp,tp,false,false,POS_FACEUP)
	end
end
