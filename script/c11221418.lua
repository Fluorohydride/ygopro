--武神隠
function c11221418.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c11221418.target)
	e1:SetOperation(c11221418.activate)
	c:RegisterEffect(e1)
end
function c11221418.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x88) and c:IsType(TYPE_XYZ) and c:IsAbleToRemove()
		and Duel.IsExistingMatchingCard(Card.IsAbleToHand,0,LOCATION_MZONE,LOCATION_MZONE,1,c)
end
function c11221418.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c11221418.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c11221418.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c11221418.filter,tp,LOCATION_MZONE,0,1,1,nil)
	local tg=Duel.GetMatchingGroup(Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,g:GetFirst())
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,tg,tg:GetCount(),0,0)
end
function c11221418.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)~=0 then
		tc:RegisterFlagEffect(11221418,RESET_EVENT+0x1fe0000,0,0)
		local g=Duel.GetMatchingGroup(Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
		if g:GetCount()==0 then return end
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE_START+PHASE_END)
		e1:SetOperation(c11221418.countop)
		if Duel.GetTurnPlayer()==tp and Duel.GetCurrentPhase()==PHASE_END then
			e1:SetLabel(1)
		else
			e1:SetLabel(0)
		end
		e1:SetReset(RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
		e1:SetLabelObject(tc)
		Duel.RegisterEffect(e1,tp)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_FIELD)
		e2:SetRange(LOCATION_SZONE)
		e2:SetCode(EFFECT_CANNOT_SUMMON)
		e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e2:SetTargetRange(1,1)
		e2:SetCondition(c11221418.limcon)
		e2:SetLabelObject(e1)
		e2:SetReset(RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
		Duel.RegisterEffect(e2,tp)
		local e3=e2:Clone()
		e3:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
		Duel.RegisterEffect(e3,tp)
		local e4=e3:Clone()
		e4:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
		Duel.RegisterEffect(e4,tp)
		local e5=Effect.CreateEffect(c)
		e5:SetType(EFFECT_TYPE_FIELD)
		e5:SetCode(EFFECT_CHANGE_DAMAGE)
		e5:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e5:SetTargetRange(1,1)
		e5:SetValue(0)
		e5:SetLabelObject(e1)
		e5:SetReset(RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
		Duel.RegisterEffect(e5,tp)
		local e6=Effect.CreateEffect(c)
		e6:SetCategory(CATEGORY_SPECIAL_SUMMON)
		e6:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
		e6:SetCode(EVENT_PHASE+PHASE_END)
		e6:SetProperty(EFFECT_FLAG_CARD_TARGET)
		e6:SetCountLimit(1)
		e6:SetCondition(c11221418.spcon)
		e6:SetTarget(c11221418.sptg)
		e6:SetOperation(c11221418.spop)
		e6:SetLabelObject(e1)
		e6:SetReset(RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
		Duel.RegisterEffect(e6,tp)
	end
end
function c11221418.limcon(e)
	return e:GetLabelObject():GetLabel()<3
end
function c11221418.countop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return end
	local ct=e:GetLabel()
	e:SetLabel(ct+1)
end
function c11221418.spcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and e:GetLabelObject():GetLabel()==2
end
function c11221418.mfilter(c)
	return c:IsSetCard(0x88) and c:IsType(TYPE_MONSTER)
end
function c11221418.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c11221418.mfilter(chkc) end
	local tc=e:GetLabelObject():GetLabelObject()
	if chk==0 then return tc:GetFlagEffect(11221418)~=0 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	local g=Duel.SelectTarget(tp,c11221418.mfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_LEAVE_GRAVE,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,tc,1,0,0)
	e:GetLabelObject():SetLabel(3)
end
function c11221418.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject():GetLabelObject()
	local mc=Duel.GetFirstTarget()
	if mc and tc:GetFlagEffect(11221418)~=0 and Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)~=0 then
		if mc:IsRelateToEffect(e) then
			Duel.Overlay(tc,Group.FromCards(mc))
		end
	end
end
