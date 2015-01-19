--セイヴァー・スター・ドラゴン
function c7841112.initial_effect(c)
	--synchro summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(c7841112.syncon)
	e1:SetOperation(c7841112.synop)
	e1:SetValue(SUMMON_TYPE_SYNCHRO)
	c:RegisterEffect(e1)
	--Negate
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(7841112,0))
	e2:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_CHAINING)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c7841112.negcon)
	e2:SetCost(c7841112.negcost)
	e2:SetTarget(c7841112.negtg)
	e2:SetOperation(c7841112.negop)
	c:RegisterEffect(e2)
	--Disable
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(7841112,1))
	e3:SetCategory(CATEGORY_DISABLE)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCountLimit(1)
	e3:SetRange(LOCATION_MZONE)
	e3:SetTarget(c7841112.distg)
	e3:SetOperation(c7841112.disop)
	c:RegisterEffect(e3)
	--activate limit
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EVENT_CHAINING)
	e4:SetRange(LOCATION_MZONE)
	e4:SetOperation(c7841112.alop)
	c:RegisterEffect(e4)
	--to extra & Special summon
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(7841112,2))
	e5:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_FIELD)
	e5:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e5:SetRange(LOCATION_MZONE)
	e5:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e5:SetCountLimit(1)
	e5:SetCode(EVENT_PHASE+PHASE_END)
	e5:SetTarget(c7841112.sptg)
	e5:SetOperation(c7841112.spop)
	c:RegisterEffect(e5)
end
function c7841112.matfilter(c,syncard)
	return c:IsFaceup() and c:IsCanBeSynchroMaterial(syncard)
end
function c7841112.synfilter1(c,syncard,lv,g)
	if not c:IsCode(21159309) then return false end
	local tlv=c:GetSynchroLevel(syncard)
	if lv-tlv<=0 then return false end
	local t=false
	if c:IsType(TYPE_TUNER) then t=true end
	local wg=g:Clone()
	wg:RemoveCard(c)
	return wg:IsExists(c7841112.synfilter2,1,nil,syncard,lv-tlv,wg,t)
end
function c7841112.synfilter2(c,syncard,lv,g,tuner)
	if not c:IsCode(44508094) then return false end
	local tlv=c:GetSynchroLevel(syncard)
	if lv-tlv<=0 then return false end
	if not tuner and not c:IsType(TYPE_TUNER) then return false end
	return g:IsExists(c7841112.synfilter3,1,c,syncard,lv-tlv)
end
function c7841112.synfilter3(c,syncard,lv)
	local mlv=c:GetSynchroLevel(syncard)
	local lv1=bit.band(mlv,0xffff)
	local lv2=bit.rshift(mlv,16)
	return c:IsNotTuner() and (lv1==lv or lv2==lv)
end
function c7841112.syncon(e,c,tuner)
	if c==nil then return true end
	local tp=c:GetControler()
	local mg=Duel.GetMatchingGroup(c7841112.matfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,c)
	local lv=c:GetLevel()
	if tuner then return c7841112.synfilter1(tuner,c,lv,mg) end
	return mg:IsExists(c7841112.synfilter1,1,nil,c,lv,mg)
end
function c7841112.synop(e,tp,eg,ep,ev,re,r,rp,c,tuner)
	local g=Group.CreateGroup()
	local mg=Duel.GetMatchingGroup(c7841112.matfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,c)
	local lv=c:GetLevel()
	local m1=tuner
	if not tuner then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
		local t1=mg:FilterSelect(tp,c7841112.synfilter1,1,1,nil,c,lv,mg)
		m1=t1:GetFirst()
		g:AddCard(m1)
	end
	lv=lv-m1:GetSynchroLevel(c)
	local t=false
	if m1:IsType(TYPE_TUNER) then t=true end
	mg:RemoveCard(m1)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local t2=mg:FilterSelect(tp,c7841112.synfilter2,1,1,nil,c,lv,mg,t)
	local m2=t2:GetFirst()
	g:AddCard(m2)
	lv=lv-m2:GetSynchroLevel(c)
	mg:RemoveCard(m2)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local t3=mg:FilterSelect(tp,c7841112.synfilter3,1,1,nil,c,lv)
	g:Merge(t3)
	c:SetMaterial(g)
	Duel.SendtoGrave(g,REASON_MATERIAL+REASON_SYNCHRO)
end
function c7841112.negcon(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsStatus(STATUS_BATTLE_DESTROYED) and ep~=tp and Duel.IsChainNegatable(ev)
end
function c7841112.negcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c7841112.negtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c7841112.negop(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.Destroy(g,REASON_EFFECT)
end
function c7841112.disfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_EFFECT) and not c:IsDisabled()
end
function c7841112.distg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c7841112.disfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c7841112.disfilter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c7841112.disfilter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,g,1,0,0)
end
function c7841112.disop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_DISABLE_EFFECT)
		e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e2)
		Duel.MajesticCopy(c,tc)
	end
end
function c7841112.alop(e,tp,eg,ep,ev,re,r,rp)
	if re:GetOwner()==e:GetOwner() and not re:IsHasProperty(EFFECT_FLAG_INITIAL) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetCode(EFFECT_CANNOT_ACTIVATE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetTargetRange(1,1)
		e1:SetValue(c7841112.aclimit)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e:GetHandler():RegisterEffect(e1)
	end
end
function c7841112.aclimit(e,re,tp)
	return re:GetOwner()==e:GetOwner() and not re:IsHasProperty(EFFECT_FLAG_INITIAL)
end
function c7841112.spfilter(c,e,tp)
	return c:IsCode(44508094) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c7841112.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c7841112.spfilter(chkc,e,tp) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c7841112.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c7841112.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if bit.band(c:GetOriginalType(),0x802040)~=0 and Duel.SendtoDeck(c,nil,0,REASON_EFFECT)~=0
		and c:IsLocation(LOCATION_EXTRA) and tc and tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
