--冥王竜ヴァンダルギオン
function c24857466.initial_effect(c)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_NEGATED)
	e2:SetRange(LOCATION_HAND)
	e2:SetOperation(c24857466.chop)
	c:RegisterEffect(e2)
	--special summon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(24857466,0))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DELAY)
	e3:SetCode(EVENT_CHAIN_END)
	e3:SetRange(LOCATION_HAND)
	e3:SetCondition(c24857466.hspcon)
	e3:SetTarget(c24857466.hsptg)
	e3:SetOperation(c24857466.hspop)
	c:RegisterEffect(e3)
	--spell:damage
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(24857466,1))
	e4:SetCategory(CATEGORY_DAMAGE)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(24857466)
	e4:SetCondition(c24857466.damcon)
	e4:SetTarget(c24857466.damtg)
	e4:SetOperation(c24857466.damop)
	c:RegisterEffect(e4)
	--trap:Destroy
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(24857466,2))
	e5:SetCategory(CATEGORY_DESTROY)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e5:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e5:SetCode(24857466)
	e5:SetCondition(c24857466.descon)
	e5:SetTarget(c24857466.destg)
	e5:SetOperation(c24857466.desop)
	c:RegisterEffect(e5)
	--monster:spsummon
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(24857466,3))
	e6:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e6:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e6:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e6:SetCode(24857466)
	e6:SetCondition(c24857466.spcon)
	e6:SetTarget(c24857466.sptg)
	e6:SetOperation(c24857466.spop)
	c:RegisterEffect(e6)
end
function c24857466.chop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if rp==tp then return end
	local de,dp=Duel.GetChainInfo(ev,CHAININFO_DISABLE_REASON,CHAININFO_DISABLE_PLAYER)
	if de and dp==tp and de:GetHandler():IsType(TYPE_COUNTER) then
		local ty=re:GetActiveType()
		local flag=c:GetFlagEffectLabel(24857466)
		if not flag then
			c:RegisterFlagEffect(24857466,RESET_EVENT+0x1fe0000,0,0,ty)
			e:SetLabelObject(de)
		elseif de~=e:GetLabelObject() then
			e:SetLabelObject(de)
			c:SetFlagEffectLabel(24857466,ty)
		else
			c:SetFlagEffectLabel(24857466,bit.bor(flag,ty))
		end
	end
end
function c24857466.hspcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local label=c:GetFlagEffectLabel(24857466)
	if label~=nil and label~=0 then
		e:SetLabel(label)
		c:SetFlagEffectLabel(24857466,0)
		return true
	else return false end
end
function c24857466.hsptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c24857466.hspop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)>0 then
		local tpe=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
		Duel.RaiseSingleEvent(c,24857466,e,0,0,tp,tpe)
	end
end
function c24857466.damcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(ev,TYPE_SPELL)~=0
end
function c24857466.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(1500)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,1500)
end
function c24857466.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
function c24857466.descon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(ev,TYPE_TRAP)~=0
end
function c24857466.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c24857466.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c24857466.spcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(ev,TYPE_MONSTER)~=0
end
function c24857466.spfilter(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c24857466.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c24857466.spfilter(chkc,e,tp) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c24857466.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c24857466.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
