--魔王ディアボロス
function c29424328.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--tribute limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_TRIBUTE_LIMIT)
	e2:SetValue(c29424328.tlimit)
	c:RegisterEffect(e2)
	--release limit
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_UNRELEASABLE_EFFECT)
	e3:SetValue(1)
	c:RegisterEffect(e3)
	--confirm deck
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(29424328,0))
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCode(EVENT_PREDRAW)
	e4:SetCondition(c29424328.cfcon)
	e4:SetOperation(c29424328.cfop)
	c:RegisterEffect(e4)
end
function c29424328.tlimit(e,c)
	return not c:IsAttribute(ATTRIBUTE_DARK)
end
function c29424328.cfcon(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer() and Duel.GetFieldGroupCount(1-tp,LOCATION_DECK,0)>0
end
function c29424328.cfop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetDecktopGroup(1-tp,1)
	Duel.ConfirmCards(tp,g)
	local tc=g:GetFirst()
	local opt=Duel.SelectOption(tp,aux.Stringid(29424328,1),aux.Stringid(29424328,2))
	if opt==1 then
		Duel.MoveSequence(tc,opt)
	end
end
