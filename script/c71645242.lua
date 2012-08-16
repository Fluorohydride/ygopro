--ブラック·ガーデン
function c71645242.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--token
	local g=Group.CreateGroup()
	g:KeepAlive()
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(71645242,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetCondition(c71645242.spcon)
	e2:SetTarget(c71645242.sptg)
	e2:SetOperation(c71645242.spop)
	e2:SetLabelObject(g)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetLabelObject(g)
	c:RegisterEffect(e3)
	--special summon
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(71645242,1))
	e4:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_SZONE)
	e4:SetTarget(c71645242.sptg2)
	e4:SetOperation(c71645242.spop2)
	c:RegisterEffect(e4)
end
function c71645242.spcon(e,tp,eg,ep,ev,re,r,rp)
	if eg:GetFirst():GetSummonType()~=SUMMON_TYPE_SPECIAL+0x20 then
		e:GetLabelObject():Clear()
		e:GetLabelObject():Merge(eg)
		return true
	else return false end
end
function c71645242.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return not e:GetHandler():IsStatus(STATUS_CHAINING) end
	Duel.SetTargetCard(e:GetLabelObject())
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c71645242.spop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	local tc=g:GetFirst()
	if not tc then return end
	local s0=false
	local s1=false
	while tc do
		if tc:IsControler(tp) then s0=true
		else s1=true end
		if tc:IsFaceup() then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_SET_ATTACK_FINAL)
			e1:SetValue(tc:GetAttack()/2)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
		end
		tc=g:GetNext()
	end
	if s0 and Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,71645243,0,0x4011,800,800,2,RACE_PLANT,ATTRIBUTE_DARK,POS_FACEUP_ATTACK,1-tp) then
		local token=Duel.CreateToken(tp,71645243)
		Duel.SpecialSummonStep(token,0x20,tp,1-tp,false,false,POS_FACEUP_ATTACK)
	end
	if s1 and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
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
	return c:GetAttack()==atk and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c71645242.sptg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c71645242.filter2(chkc,e:GetLabel(),e,tp) end
	local g=Duel.GetMatchingGroup(c71645242.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local atk=g:GetSum(Card.GetAttack)
	local sc=g:FilterCount(Card.IsControler,nil,tp)
	if chk==0 then return e:GetHandler():IsDestructable(e) and g:GetCount()>0 and (Duel.GetLocationCount(tp,LOCATION_MZONE)>-sc)
		and Duel.IsExistingMatchingCard(c71645242.filter2,tp,LOCATION_GRAVE,0,1,nil,atk,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local tg=Duel.SelectTarget(tp,c71645242.filter2,tp,LOCATION_GRAVE,0,1,1,nil,atk,e,tp)
	e:SetLabel(atk)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,tg,1,0,0)
	g:AddCard(e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c71645242.spop2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local dg=Duel.GetMatchingGroup(c71645242.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local atk=dg:GetSum(Card.GetAttack)
	dg:AddCard(e:GetHandler())
	Duel.Destroy(dg,REASON_EFFECT)
	Duel.BreakEffect()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:GetAttack()==atk then
		Duel.SpecialSummon(tc,0x20,tp,tp,false,false,POS_FACEUP)
	end
end
