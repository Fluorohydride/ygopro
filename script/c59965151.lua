--デッド・ガードナー
function c59965151.initial_effect(c)
	--change battle target
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(59965151,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c59965151.cbcon)
	e1:SetOperation(c59965151.cbop)
	c:RegisterEffect(e1)
	--atkdown
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(59965151,1))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCondition(c59965151.atkcon)
	e2:SetTarget(c59965151.atktg)
	e2:SetOperation(c59965151.atkop)
	c:RegisterEffect(e2)
end
function c59965151.cbcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bt=eg:GetFirst()
	return c~=bt and bt:IsFaceup() and bt:GetControler()==c:GetControler()
end
function c59965151.cbop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeAttackTarget(e:GetHandler())
end
function c59965151.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsReason(REASON_DESTROY)
end
function c59965151.atktg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_MZONE) and chkc:IsFaceup() end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,0,LOCATION_MZONE,1,1,nil)
end
function c59965151.atkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(-1000)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
	end
end
