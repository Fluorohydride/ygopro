--デスルークデーモン
function c72192100.initial_effect(c)
	--maintain
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetOperation(c72192100.mtop)
	c:RegisterEffect(e1)
	--disable and destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c72192100.disop)
	c:RegisterEffect(e2)
	--special summon
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetDescription(aux.Stringid(72192100,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetRange(LOCATION_HAND)
	e3:SetCost(c72192100.spcost)
	e3:SetTarget(c72192100.sptg)
	e3:SetOperation(c72192100.spop)
	c:RegisterEffect(e3)
end
function c72192100.mtop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return end
	if Duel.CheckLPCost(tp,500) then
		Duel.PayLPCost(tp,500)
	else
		Duel.Destroy(e:GetHandler(),REASON_RULE)
	end
end
function c72192100.disop(e,tp,eg,ep,ev,re,r,rp)
	if ep==tp then return end
	if not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return false end
	local tg=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
	if not tg or not tg:IsContains(e:GetHandler()) or not Duel.IsChainDisablable(ev) then return false end
	local rc=re:GetHandler()
	local dc=Duel.TossDice(tp,1)
	if dc~=3 then return end
	Duel.NegateEffect(ev)
	if rc:IsRelateToEffect(re) then
		Duel.Destroy(rc,REASON_EFFECT)
	end
end
function c72192100.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c72192100.filter(c,e,tp)
	return c:IsCode(35975813) and c:GetPreviousControler()==tp and c:IsPreviousLocation(LOCATION_ONFIELD)
		and c:IsCanBeEffectTarget(e) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c72192100.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return eg:IsContains(chkc) and c72192100.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and eg:IsExists(c72192100.filter,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=eg:FilterSelect(tp,c72192100.filter,1,1,nil,e,tp)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c72192100.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
