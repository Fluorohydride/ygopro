--クリアー・ワールド
function c33900648.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--maintain
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c33900648.mtcon)
	e2:SetOperation(c33900648.mtop)
	c:RegisterEffect(e2)
	--adjust
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_ADJUST)
	e3:SetRange(LOCATION_SZONE)
	e3:SetOperation(c33900648.adjustop)
	c:RegisterEffect(e3)
	--light
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_PUBLIC)
	e4:SetRange(LOCATION_SZONE)
	e4:SetTargetRange(LOCATION_HAND,LOCATION_HAND)
	e4:SetTarget(c33900648.lighttg)
	c:RegisterEffect(e4)
	--dark
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD)
	e5:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e5:SetRange(LOCATION_SZONE)
	e5:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e5:SetTargetRange(1,0)
	e5:SetCondition(c33900648.darkcon1)
	c:RegisterEffect(e5)
	local e6=e5:Clone()
	e6:SetCondition(c33900648.darkcon2)
	e6:SetTargetRange(0,1)
	c:RegisterEffect(e6)
	--earth
	local e7=Effect.CreateEffect(c)
	e7:SetDescription(aux.Stringid(33900648,1))
	e7:SetCategory(CATEGORY_DESTROY)
	e7:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e7:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e7:SetRange(LOCATION_SZONE)
	e7:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e7:SetCountLimit(1)
	e7:SetCondition(c33900648.descon)
	e7:SetTarget(c33900648.destg)
	e7:SetOperation(c33900648.desop)
	c:RegisterEffect(e7)
	--water
	local e8=Effect.CreateEffect(c)
	e8:SetDescription(aux.Stringid(33900648,2))
	e8:SetCategory(CATEGORY_HANDES)
	e8:SetCode(EVENT_PHASE+PHASE_END)
	e8:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e8:SetRange(LOCATION_SZONE)
	e8:SetCountLimit(1)
	e8:SetCondition(c33900648.hdcon)
	e8:SetTarget(c33900648.hdtg)
	e8:SetOperation(c33900648.hdop)
	c:RegisterEffect(e8)
	--fire
	local e9=Effect.CreateEffect(c)
	e9:SetDescription(aux.Stringid(33900648,3))
	e9:SetCategory(CATEGORY_DAMAGE)
	e9:SetCode(EVENT_PHASE+PHASE_END)
	e9:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e9:SetRange(LOCATION_SZONE)
	e9:SetCountLimit(1)
	e9:SetCondition(c33900648.damcon)
	e9:SetTarget(c33900648.damtg)
	e9:SetOperation(c33900648.damop)
	c:RegisterEffect(e9)
	--wind
	local e10=Effect.CreateEffect(c)
	e10:SetType(EFFECT_TYPE_FIELD)
	e10:SetCode(EFFECT_ACTIVATE_COST)
	e10:SetRange(LOCATION_SZONE)
	e10:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e10:SetTargetRange(1,0)
	e10:SetTarget(c33900648.actarget)
	e10:SetCondition(c33900648.windcon1)
	e10:SetCost(c33900648.costchk)
	e10:SetOperation(c33900648.costop)
	c:RegisterEffect(e10)
	local e11=e10:Clone()
	e11:SetTargetRange(0,1)
	e11:SetCondition(c33900648.windcon2)
	c:RegisterEffect(e11)
end
function c33900648.mtcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c33900648.mtop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLP(tp)>500 and Duel.SelectYesNo(tp,aux.Stringid(33900648,0)) then
		Duel.PayLPCost(tp,500)
	else
		Duel.Destroy(e:GetHandler(),REASON_RULE)
	end
end
c33900648[0]=0
c33900648[1]=0
function c33900648.raccheck(p)
	local rac=0
	for i=0,4 do
		local tc=Duel.GetFieldCard(p,LOCATION_MZONE,i)
		if tc and tc:IsFaceup() then
			rac=bit.bor(rac,tc:GetAttribute())
		end
	end
	c33900648[p]=rac
end
function c33900648.adjustop(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsPlayerAffectedByEffect(0,97811903) then
		c33900648.raccheck(0)
	else c33900648[0]=0 end
	if not Duel.IsPlayerAffectedByEffect(1,97811903) then
		c33900648.raccheck(1)
	else c33900648[1]=0 end
end
function c33900648.lighttg(e,c)
	return bit.band(c33900648[c:GetControler()],ATTRIBUTE_LIGHT)~=0
end
function c33900648.darkcon1(e)
	return bit.band(c33900648[e:GetHandlerPlayer()],ATTRIBUTE_DARK)~=0
		and Duel.GetFieldGroupCount(e:GetHandlerPlayer(),LOCATION_MZONE,0)>=2
end
function c33900648.darkcon2(e)
	return bit.band(c33900648[1-e:GetHandlerPlayer()],ATTRIBUTE_DARK)~=0
		and Duel.GetFieldGroupCount(e:GetHandlerPlayer(),0,LOCATION_MZONE)>=2
end
function c33900648.descon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(c33900648[Duel.GetTurnPlayer()],ATTRIBUTE_EARTH)~=0
end
function c33900648.desfilter(c)
	return c:IsPosition(POS_FACEUP_DEFENCE) and c:IsDestructable()
end
function c33900648.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local turnp=Duel.GetTurnPlayer()
	if chkc then return chkc:IsControler(turnp) and chkc:IsLocation(LOCATION_MZONE) and c33900648.desfilter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(turnp,c33900648.desfilter,turnp,LOCATION_MZONE,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
end
function c33900648.desop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if bit.band(c33900648[Duel.GetTurnPlayer()],ATTRIBUTE_EARTH)==0 then return end
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsPosition(POS_FACEUP_DEFENCE) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c33900648.hdcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(c33900648[Duel.GetTurnPlayer()],ATTRIBUTE_WATER)~=0
end
function c33900648.hdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local turnp=Duel.GetTurnPlayer()
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,turnp,1)
end
function c33900648.hdop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if bit.band(c33900648[Duel.GetTurnPlayer()],ATTRIBUTE_WATER)==0 then return end
	Duel.DiscardHand(Duel.GetTurnPlayer(),nil,1,1,REASON_EFFECT+REASON_DISCARD)
end
function c33900648.damcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(c33900648[Duel.GetTurnPlayer()],ATTRIBUTE_FIRE)~=0
end
function c33900648.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local turnp=Duel.GetTurnPlayer()
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,turnp,1000)
end
function c33900648.damop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if bit.band(c33900648[Duel.GetTurnPlayer()],ATTRIBUTE_FIRE)==0 then return end
	Duel.Damage(Duel.GetTurnPlayer(),1000,REASON_EFFECT)
end
function c33900648.windcon1(e)
	return bit.band(c33900648[e:GetHandlerPlayer()],ATTRIBUTE_WIND)~=0
end
function c33900648.windcon2(e)
	return bit.band(c33900648[1-e:GetHandlerPlayer()],ATTRIBUTE_WIND)~=0
end
function c33900648.actarget(e,te,tp)
	return te:IsHasType(EFFECT_TYPE_ACTIVATE) and te:IsActiveType(TYPE_SPELL)
end
function c33900648.costchk(e,te_or_c,tp)
	return Duel.CheckLPCost(tp,500)
end
function c33900648.costop(e,tp,eg,ep,ev,re,r,rp)
	Duel.PayLPCost(tp,500)
end
